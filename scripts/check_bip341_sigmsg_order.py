#!/usr/bin/env python3
"""Check that the provisional QRS SigMsg model preserves BIP-341 field order.

The QRS vectors are still provisional and blocked on final BIP-360/QRS
definitions, but the supported SIGHASH_DEFAULT subset must not regress to the
previous toy ordering where spend_type appeared before nVersion. For the
supported subset, spend_type belongs after the transaction-wide BIP-341 hashes
and before input_index.
"""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

import compute_qrs_digest_model as model


def tx_wide_payloads(data: dict, input_count: int, path: Path) -> tuple[bytes, bytes, bytes, bytes, bytes]:
    tx = data.get("transaction", {})
    inputs = tx.get("inputs", [])
    outputs = tx.get("outputs", [])
    prevouts_payload = b"".join(model.parse_prevout(item["prevout"], path) for item in inputs)
    amounts_payload = b"".join(
        model.ser_u64(int(item.get("amount_sats", item.get("amount_sat", 0))))
        for item in model.spent_outputs(data, input_count, path)
    )
    scriptpubkeys_payload = b"".join(
        model.serialize_scriptpubkey(
            model.script_bytes(item["script_pubkey"], path, "spent_outputs[].script_pubkey")
        )
        for item in model.spent_outputs(data, input_count, path)
    )
    sequences_payload = b"".join(model.ser_u32(int(item.get("sequence", 0))) for item in inputs)
    outputs_payload = b"".join(
        model.ser_u64(int(item["value_sat"]))
        + model.serialize_scriptpubkey(
            model.script_bytes(item["script_pubkey"], path, "transaction.outputs[].script_pubkey")
        )
        for item in outputs
    )
    return prevouts_payload, amounts_payload, scriptpubkeys_payload, sequences_payload, outputs_payload


def manual_sigmsg(data: dict, leaf_hash: bytes, annex: bytes, path: Path) -> bytes:
    tx = data.get("transaction", {})
    inputs = tx.get("inputs", [])
    if not isinstance(inputs, list) or not inputs:
        raise AssertionError(f"{path}: transaction.inputs must be non-empty")
    input_index = int(data.get("input_index", 0))
    annex_present = 1 if annex else 0
    spend_type = model.QRS_EXT_FLAG * 2 + annex_present
    prevouts, amounts, scripts, sequences, outputs = tx_wide_payloads(data, len(inputs), path)

    preimage = (
        bytes([model.QRS_HASH_TYPE])
        + model.ser_u32(int(tx.get("version", 0)))
        + model.ser_u32(int(tx.get("lock_time", 0)))
        + hashlib.sha256(prevouts).digest()
        + hashlib.sha256(amounts).digest()
        + hashlib.sha256(scripts).digest()
        + hashlib.sha256(sequences).digest()
        + hashlib.sha256(outputs).digest()
        + bytes([spend_type])
        + model.ser_u32(input_index)
    )
    if annex:
        preimage += hashlib.sha256(model.compact_size(len(annex)) + annex).digest()
    return preimage + leaf_hash


def legacy_wrong_sigmsg(data: dict, leaf_hash: bytes, annex: bytes, path: Path) -> bytes:
    tx = data.get("transaction", {})
    inputs = tx.get("inputs", [])
    input_index = int(data.get("input_index", 0))
    annex_present = 1 if annex else 0
    spend_type = model.QRS_EXT_FLAG * 2 + annex_present
    prevouts, amounts, scripts, sequences, outputs = tx_wide_payloads(data, len(inputs), path)

    preimage = (
        bytes([model.QRS_HASH_TYPE, spend_type])
        + model.ser_u32(int(tx.get("version", 0)))
        + model.ser_u32(int(tx.get("lock_time", 0)))
        + hashlib.sha256(prevouts).digest()
        + hashlib.sha256(amounts).digest()
        + hashlib.sha256(scripts).digest()
        + hashlib.sha256(sequences).digest()
        + hashlib.sha256(outputs).digest()
        + model.ser_u32(input_index)
    )
    if annex:
        preimage += hashlib.sha256(model.compact_size(len(annex)) + annex).digest()
    return preimage + leaf_hash


def check_vector(path: Path, data: dict) -> None:
    pubkey = model.expand_descriptor(data["qrs_public_key"], path, "qrs_public_key")
    annex = model.effective_annex(data, path)
    leaf_hash = model.modeled_leaf_hash(pubkey)
    expected = model.hex_bytes(data["expected_qrs_msg"], path, "expected_qrs_msg")
    spend_type = model.QRS_EXT_FLAG * 2 + (1 if annex else 0)

    built = model.build_sigmsg(data, leaf_hash, annex, path)
    manual = manual_sigmsg(data, leaf_hash, annex, path)
    if built != manual:
        raise AssertionError(f"{path}: build_sigmsg does not match manual BIP-341 field order")

    if manual[0] != model.QRS_HASH_TYPE:
        raise AssertionError(f"{path}: hash_type is not the first SigMsg byte")
    tx = data.get("transaction", {})
    if manual[1:5] != model.ser_u32(int(tx.get("version", 0))):
        raise AssertionError(f"{path}: nVersion does not immediately follow hash_type")
    spend_type_offset = 1 + 4 + 4 + 32 * 5
    if manual[spend_type_offset] != spend_type:
        raise AssertionError(f"{path}: spend_type is not after transaction-wide BIP-341 hashes")
    if manual[spend_type_offset + 1 : spend_type_offset + 5] != model.ser_u32(int(data.get("input_index", 0))):
        raise AssertionError(f"{path}: input_index does not immediately follow spend_type")

    digest = model.tagged_hash("TapSighash", b"\x00" + manual)
    if digest != expected:
        raise AssertionError(f"{path}: manually ordered SigMsg does not match expected_qrs_msg")

    wrong = legacy_wrong_sigmsg(data, leaf_hash, annex, path)
    wrong_digest = model.tagged_hash("TapSighash", b"\x00" + wrong)
    if wrong == manual or wrong_digest == expected:
        raise AssertionError(f"{path}: legacy spend_type-before-nVersion order was not rejected")


def main() -> int:
    vectors_dir = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("test_vectors")
    vectors = model.load_vectors(vectors_dir)
    for path, data in vectors:
        check_vector(path, data)
    print(f"BIP-341 SigMsg order self-test passed for {len(vectors)} vectors")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (AssertionError, KeyError, ValueError, model.DigestModelError) as exc:
        print(f"check_bip341_sigmsg_order.py: {exc}", file=sys.stderr)
        raise SystemExit(1)
