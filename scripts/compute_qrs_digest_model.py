#!/usr/bin/env python3
"""Compute the provisional byte-level QRS digest model for draft vectors.

This is not final Bitcoin Core SigMsg code. It is a byte-oriented, QRS-shaped
model that tracks the BIP-341 common signature-message structure closely enough
for pre-review: hash type, spend type, transaction fields, prevout/amount/
scriptPubKey/sequence/output digests, input index, optional annex digest, and
qrs_ext = leaf_hash.
"""

from __future__ import annotations

import argparse
import copy
import hashlib
import json
import re
import sys
from pathlib import Path
from typing import Any


QRS_LEAF_VERSION = 0xC2
QRS_HASH_TYPE = 0x00
QRS_EXT_FLAG = 0x02
P2MR_SCRIPT_PREFIX = "5120"


class DigestModelError(AssertionError):
    pass


def fail(path: Path, message: str) -> None:
    raise DigestModelError(f"{path}: {message}")


def tagged_hash(tag: str, payload: bytes) -> bytes:
    tag_hash = hashlib.sha256(tag.encode("ascii")).digest()
    return hashlib.sha256(tag_hash + tag_hash + payload).digest()


def compact_size(value: int) -> bytes:
    if value < 0:
        raise ValueError("compact_size cannot encode a negative value")
    if value < 253:
        return bytes([value])
    if value <= 0xFFFF:
        return b"\xfd" + value.to_bytes(2, "little")
    if value <= 0xFFFFFFFF:
        return b"\xfe" + value.to_bytes(4, "little")
    if value <= 0xFFFFFFFFFFFFFFFF:
        return b"\xff" + value.to_bytes(8, "little")
    raise ValueError("compact_size value too large")


def hex_bytes(value: str, path: Path, field: str) -> bytes:
    if not isinstance(value, str) or re.fullmatch(r"[0-9a-f]*", value) is None or len(value) % 2:
        fail(path, f"{field} must be even-length lowercase hex")
    return bytes.fromhex(value)


def expand_descriptor(value: object, path: Path, field: str) -> bytes:
    if isinstance(value, str):
        return hex_bytes(value, path, field)
    if not isinstance(value, dict):
        fail(path, f"{field} must be hex or a descriptor object")

    if "hex" in value:
        out = hex_bytes(value["hex"], path, f"{field}.hex")
        if "bytes" in value and int(value["bytes"]) != len(out):
            fail(path, f"{field}.bytes does not match {field}.hex")
        return out

    encoding = value.get("encoding")
    if encoding == "repeat_byte":
        byte = hex_bytes(value.get("byte"), path, f"{field}.byte")
        if len(byte) != 1:
            fail(path, f"{field}.byte must be exactly one byte")
        return byte * int(value["bytes"])

    if encoding == "repeat_sibling_path":
        control = hex_bytes(value.get("control_byte"), path, f"{field}.control_byte")
        sibling = hex_bytes(value.get("sibling_hash_byte"), path, f"{field}.sibling_hash_byte")
        if len(control) != 1 or len(sibling) != 1:
            fail(path, f"{field} repeat_sibling_path requires one-byte control and sibling bytes")
        depth = int(value["depth"])
        out = control + sibling * (32 * depth)
        if int(value.get("bytes", len(out))) != len(out):
            fail(path, f"{field}.bytes does not match repeat_sibling_path depth")
        return out

    fail(path, f"{field} has unsupported descriptor encoding {encoding!r}")
    return b""


def ser_u32(value: int) -> bytes:
    if value < 0 or value > 0xFFFFFFFF:
        raise ValueError("u32 out of range")
    return value.to_bytes(4, "little")


def ser_u64(value: int) -> bytes:
    if value < 0 or value > 0xFFFFFFFFFFFFFFFF:
        raise ValueError("u64 out of range")
    return value.to_bytes(8, "little")


def modeled_leaf_hash(pubkey: bytes) -> bytes:
    return tagged_hash("TapLeaf", bytes([QRS_LEAF_VERSION]) + compact_size(len(pubkey)) + pubkey)


def merkle_root(leaf_hash: bytes, control_block: bytes) -> bytes:
    root = leaf_hash
    for offset in range(1, len(control_block), 32):
        sibling = control_block[offset : offset + 32]
        left, right = sorted([root, sibling])
        root = tagged_hash("TapBranch", left + right)
    return root


def parse_prevout(prevout: str, path: Path) -> bytes:
    if not isinstance(prevout, str) or ":" not in prevout:
        fail(path, "transaction input prevout must be hex:index")
    txid_hex, vout_text = prevout.split(":", 1)
    txid = hex_bytes(txid_hex, path, "transaction.inputs[].prevout.txid")
    if len(txid) == 1:
        txid = txid * 32
    if len(txid) != 32:
        fail(path, "transaction input prevout txid must be 32 bytes or one-byte shorthand")
    return txid + ser_u32(int(vout_text))


def script_bytes(value: str, path: Path, field: str) -> bytes:
    return hex_bytes(value, path, field)


def spent_outputs(data: dict[str, Any], input_count: int, path: Path) -> list[dict[str, Any]]:
    explicit = data.get("spent_outputs")
    if isinstance(explicit, list) and explicit:
        return explicit
    tx_outputs = data.get("transaction", {}).get("outputs", [])
    fallback_amount = int(tx_outputs[0].get("value_sat", 0)) if tx_outputs else 0
    fallback_script = data.get("spent_output_scriptPubKey", "")
    return [
        {
            "amount_sats": fallback_amount,
            "script_pubkey": fallback_script,
        }
        for _ in range(input_count)
    ]


def serialize_scriptpubkey(script: bytes) -> bytes:
    return compact_size(len(script)) + script


def build_sigmsg(data: dict[str, Any], leaf_hash: bytes, annex: bytes, path: Path) -> bytes:
    tx = data.get("transaction", {})
    inputs = tx.get("inputs", [])
    outputs = tx.get("outputs", [])
    if not isinstance(inputs, list) or not inputs:
        fail(path, "transaction.inputs must be a non-empty list")
    if not isinstance(outputs, list):
        fail(path, "transaction.outputs must be a list")

    annex_present = 1 if annex else 0
    spend_type = QRS_EXT_FLAG * 2 + annex_present
    prevouts_payload = b"".join(parse_prevout(item["prevout"], path) for item in inputs)
    amounts_payload = b"".join(
        ser_u64(int(item.get("amount_sats", item.get("amount_sat", 0))))
        for item in spent_outputs(data, len(inputs), path)
    )
    scriptpubkeys_payload = b"".join(
        serialize_scriptpubkey(script_bytes(item["script_pubkey"], path, "spent_outputs[].script_pubkey"))
        for item in spent_outputs(data, len(inputs), path)
    )
    sequences_payload = b"".join(ser_u32(int(item.get("sequence", 0))) for item in inputs)
    outputs_payload = b"".join(
        ser_u64(int(item["value_sat"]))
        + serialize_scriptpubkey(script_bytes(item["script_pubkey"], path, "transaction.outputs[].script_pubkey"))
        for item in outputs
    )
    input_index = int(data.get("input_index", 0))
    if input_index < 0 or input_index >= len(inputs):
        fail(path, "input_index must select a transaction input")

    sigmsg = (
        bytes([QRS_HASH_TYPE, spend_type])
        + ser_u32(int(tx.get("version", 0)))
        + ser_u32(int(tx.get("lock_time", 0)))
        + hashlib.sha256(prevouts_payload).digest()
        + hashlib.sha256(amounts_payload).digest()
        + hashlib.sha256(scriptpubkeys_payload).digest()
        + hashlib.sha256(sequences_payload).digest()
        + hashlib.sha256(outputs_payload).digest()
        + ser_u32(input_index)
    )
    if annex_present:
        sigmsg += hashlib.sha256(compact_size(len(annex)) + annex).digest()
    return sigmsg + leaf_hash


def compute_vector_digest(data: dict[str, Any], path: Path) -> dict[str, Any]:
    pubkey = expand_descriptor(data["qrs_public_key"], path, "qrs_public_key")
    control_block = expand_descriptor(data["control_block"], path, "control_block")
    annex = hex_bytes(data.get("annex", ""), path, "annex")
    leaf_hash = modeled_leaf_hash(pubkey)
    root = merkle_root(leaf_hash, control_block)
    sigmsg_with_ext = build_sigmsg(data, leaf_hash, annex, path)
    qrs_msg = tagged_hash("TapSighash", b"\x00" + sigmsg_with_ext)
    return {
        "leaf_hash": leaf_hash.hex(),
        "merkle_root": root.hex(),
        "qrs_msg": qrs_msg.hex(),
        "qrs_hash_type": QRS_HASH_TYPE,
        "qrs_ext_flag": QRS_EXT_FLAG,
        "annex_present": bool(annex),
        "spend_type": QRS_EXT_FLAG * 2 + (1 if annex else 0),
    }


def load_vectors(vectors_dir: Path) -> list[tuple[Path, dict[str, Any]]]:
    paths = sorted(
        path for path in vectors_dir.glob("qrs_*.json") if not path.name.endswith(".schema.json")
    )
    if not paths:
        raise DigestModelError(f"no qrs_*.json vectors found under {vectors_dir}")
    return [(path, json.loads(path.read_text(encoding="utf-8"))) for path in paths]


def assert_mutation_changes(path: Path, data: dict[str, Any], label: str, mutate) -> None:
    baseline = compute_vector_digest(data, path)["qrs_msg"]
    mutated = copy.deepcopy(data)
    mutate(mutated)
    changed = compute_vector_digest(mutated, path)["qrs_msg"]
    if changed == baseline:
        fail(path, f"mutation {label} did not change qrs_msg")


def run_mutation_self_tests(vectors: list[tuple[Path, dict[str, Any]]]) -> None:
    by_name = {data["name"]: (path, data) for path, data in vectors}
    no_annex_path, no_annex = by_name["qrs_valid_no_annex"]
    with_annex_path, with_annex = by_name["qrs_valid_with_annex"]
    assert_mutation_changes(
        no_annex_path,
        no_annex,
        "changed output amount",
        lambda d: d["transaction"]["outputs"][0].__setitem__("value_sat", d["transaction"]["outputs"][0]["value_sat"] + 1),
    )
    two_input = copy.deepcopy(no_annex)
    two_input["transaction"]["inputs"].append({"prevout": "11:9", "sequence": 1})
    assert_mutation_changes(no_annex_path, two_input, "changed input index", lambda d: d.__setitem__("input_index", 1))
    assert_mutation_changes(with_annex_path, with_annex, "changed annex", lambda d: d.__setitem__("annex", "50ab"))
    assert_mutation_changes(no_annex_path, no_annex, "changed leaf hash", lambda d: d.__setitem__("qrs_public_key", "02" * 32))
    assert_mutation_changes(
        no_annex_path,
        no_annex,
        "changed scriptPubKey",
        lambda d: d.__setitem__("spent_output_scriptPubKey", "5120" + "22" * 32),
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("vectors", type=Path)
    parser.add_argument("--update", action="store_true")
    parser.add_argument("--json", type=Path, default=None)
    args = parser.parse_args()

    vectors = load_vectors(args.vectors)
    results = []
    for path, data in vectors:
        result = compute_vector_digest(data, path)
        if args.update:
            data["expected_leaf_hash"] = result["leaf_hash"]
            data["expected_qrs_msg"] = result["qrs_msg"]
            path.write_text(json.dumps(data, indent=2, sort_keys=False) + "\n", encoding="utf-8")
        else:
            if data.get("expected_leaf_hash") != result["leaf_hash"]:
                fail(path, "expected_leaf_hash does not match byte-level QRS digest model")
            if data.get("expected_qrs_msg") != result["qrs_msg"]:
                fail(path, "expected_qrs_msg does not match byte-level QRS digest model")
        results.append({"path": str(path), **result})
    run_mutation_self_tests(vectors)
    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(results, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"computed byte-level QRS digest model for {len(vectors)} vectors; mutation self-tests passed")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (DigestModelError, AssertionError, KeyError, ValueError, json.JSONDecodeError) as exc:
        print(f"compute_qrs_digest_model.py: {exc}", file=sys.stderr)
        sys.exit(1)
