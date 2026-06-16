#!/usr/bin/env python3
"""Execute the provisional QRS structured-fixture model.

These are provisional executable structured fixtures, not final consensus
vectors: the BIP-360/P2MR leaf hashing and QRS sighash rules are still draft
material. This verifier makes the current draft structure executable by
recomputing leaf hashes, a SigMsg-shaped QRS message, and failure-stage
classification, without pretending to provide final consensus-vector coverage.
This fixture model tests the QRS message preimage shape: hash_type, spend_type,
annex commitment, transaction/spent-output commitments, and qrs_ext =
leaf_hash. It is not a final Bitcoin Core SigMsg implementation.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from pathlib import Path

from compute_qrs_digest_model import compute_vector_digest  # type: ignore


QRS_LEAF_VERSION = 0xC2
QRS_HASH_TYPE = 0x00
QRS_EXT_FLAG = 0x02
# Provisional root-carrier encoding for executable fixtures only. BIP-360 final
# P2MR output encoding is still an external blocker; do not treat this as a
# final consensus scriptPubKey prefix.
PROVISIONAL_ROOT_SCRIPT_PREFIX = "5120"
STRUCTURAL_FAILURES = {
    "signature_length",
    "public_key_length",
    "control_block_length",
    "control_low_bit",
    "leaf_version",
    "merkle_root_mismatch",
    "malformed_witness",
}


class FixtureError(AssertionError):
    pass


def fail(path: Path, message: str) -> None:
    raise FixtureError(f"{path}: {message}")


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


def modeled_leaf_hash(pubkey: bytes) -> bytes:
    return tagged_hash(
        "TapLeaf",
        bytes([QRS_LEAF_VERSION]) + compact_size(len(pubkey)) + pubkey,
    )


def bip_form_tapleaf_preimage(pubkey: bytes) -> bytes:
    return bytes([QRS_LEAF_VERSION]) + compact_size(32) + pubkey


def merkle_root(leaf_hash: bytes, control_block: bytes) -> bytes:
    root = leaf_hash
    for offset in range(1, len(control_block), 32):
        sibling = control_block[offset : offset + 32]
        left, right = sorted([root, sibling])
        root = tagged_hash("TapBranch", left + right)
    return root


def modeled_qrs_msg(data: dict, leaf_hash: bytes, annex: bytes, path: Path) -> bytes:
    del leaf_hash, annex
    return bytes.fromhex(compute_vector_digest(data, path)["qrs_msg"])


def provisional_root_from_script_pubkey(script_pubkey: str, path: Path) -> bytes:
    if not isinstance(script_pubkey, str):
        fail(path, "spent_output_scriptPubKey must be lowercase hex")
    if not script_pubkey.startswith(PROVISIONAL_ROOT_SCRIPT_PREFIX) or len(script_pubkey) != 68:
        fail(
            path,
            "spent_output_scriptPubKey must use the provisional OP_1 PUSH32 "
            "root-carrier plus a 32-byte root",
        )
    return hex_bytes(
        script_pubkey[len(PROVISIONAL_ROOT_SCRIPT_PREFIX) :],
        path,
        "spent_output_scriptPubKey root",
    )


def witness_stack_after_annex_removal(data: dict) -> list[str] | None:
    """Return the modeled QRS stack after removing the optional annex marker.

    The current provisional fixtures model witness elements by semantic label
    instead of serialized stack bytes. The optional annex, when represented in
    this label stack, must be the final label and is stripped before enforcing
    the QRS witness cardinality. Any other extra element remains visible and
    must fail as malformed witness data.
    """

    witness_stack = data.get("witness_stack")
    if not isinstance(witness_stack, list) or not all(isinstance(v, str) for v in witness_stack):
        return None
    normalized = list(witness_stack)
    if normalized and normalized[-1] == "annex":
        normalized = normalized[:-1]
    return normalized


def classify(data: dict, sig: bytes, pubkey: bytes, control_block: bytes, leaf_hash: bytes, path: Path) -> str:
    if witness_stack_after_annex_removal(data) != [
        "qrs_signature",
        "qrs_public_key",
        "control_block",
    ]:
        return "malformed_witness"

    if len(sig) != 7856:
        return "signature_length"
    if len(pubkey) != 32:
        return "public_key_length"
    if len(control_block) < 1 or (len(control_block) - 1) % 32 != 0 or len(control_block) > 4097:
        return "control_block_length"
    if (control_block[0] & 0x01) == 0:
        return "control_low_bit"
    if (control_block[0] & 0xFE) != QRS_LEAF_VERSION:
        return "leaf_version"
    if merkle_root(leaf_hash, control_block) != provisional_root_from_script_pubkey(
        data["spent_output_scriptPubKey"], path
    ):
        return "merkle_root_mismatch"
    return "none" if data["expected_result"] else "slh_dsa_verify"


def update_spend_root(data: dict, root: bytes) -> None:
    script = PROVISIONAL_ROOT_SCRIPT_PREFIX + root.hex()
    data["spent_output_scriptPubKey"] = script
    outputs = data.get("transaction", {}).get("outputs", [])
    if outputs and isinstance(outputs[0], dict):
        outputs[0]["script_pubkey"] = script


def verify_one(path: Path, update: bool) -> dict:
    data = json.loads(path.read_text(encoding="utf-8"))
    sig = expand_descriptor(data["qrs_signature"], path, "qrs_signature")
    pubkey = expand_descriptor(data["qrs_public_key"], path, "qrs_public_key")
    control_block = expand_descriptor(data["control_block"], path, "control_block")
    annex = hex_bytes(data.get("annex", ""), path, "annex")

    leaf_hash = modeled_leaf_hash(pubkey)
    qrs_msg = modeled_qrs_msg(data, leaf_hash, annex, path)

    if update:
        data["expected_leaf_hash"] = leaf_hash.hex()
        data["expected_qrs_msg"] = qrs_msg.hex()
        if data["failure_stage"] in {"none", "slh_dsa_verify"}:
            update_spend_root(data, merkle_root(leaf_hash, control_block))
            qrs_msg = modeled_qrs_msg(data, leaf_hash, annex, path)
            data["expected_qrs_msg"] = qrs_msg.hex()
        path.write_text(json.dumps(data, indent=2, sort_keys=False) + "\n", encoding="utf-8")

    if data.get("expected_leaf_hash") != leaf_hash.hex():
        fail(path, "expected_leaf_hash does not match executable fixture model")
    if data.get("expected_qrs_msg") != qrs_msg.hex():
        fail(path, "expected_qrs_msg does not match executable fixture model")

    stage = classify(data, sig, pubkey, control_block, leaf_hash, path)
    if stage != data.get("failure_stage"):
        fail(path, f"classified failure_stage {stage!r}, expected {data.get('failure_stage')!r}")
    reaches = stage in {"none", "slh_dsa_verify"}
    if bool(data.get("reaches_slh_dsa_verifier")) != reaches:
        fail(path, f"reaches_slh_dsa_verifier should be {reaches}")
    if stage == "none" and not data.get("expected_result"):
        fail(path, "stage none requires expected_result true")
    if stage != "none" and data.get("expected_result"):
        fail(path, "invalid stage requires expected_result false")
    if stage == "slh_dsa_verify" and len(sig) != 7856:
        fail(path, "SLH-DSA verifier-reaching fixture must keep a fixed 7,856-byte signature")
    if stage in STRUCTURAL_FAILURES and data.get("reaches_slh_dsa_verifier"):
        fail(path, "structural failure must not reach SLH-DSA verifier")
    return data


def verify_tapleaf_preimage_self_test(paths: list[Path]) -> None:
    for path in paths:
        data = json.loads(path.read_text(encoding="utf-8"))
        if data.get("failure_stage") not in {"none", "slh_dsa_verify"}:
            continue
        pubkey = expand_descriptor(data["qrs_public_key"], path, "qrs_public_key")
        if len(pubkey) != 32:
            continue
        expected = hex_bytes(data["expected_leaf_hash"], path, "expected_leaf_hash")
        correct = tagged_hash("TapLeaf", bip_form_tapleaf_preimage(pubkey))
        wrong_no_length = tagged_hash("TapLeaf", bytes([QRS_LEAF_VERSION]) + pubkey)
        if correct != expected:
            fail(path, "TapLeaf preimage self-test failed: BIP-form compact_size preimage mismatch")
        if wrong_no_length == expected:
            fail(path, "TapLeaf preimage self-test failed: no-length-prefix preimage matched")
        print("TapLeaf preimage self-test passed")
        return
    raise FixtureError("TapLeaf preimage self-test found no verifier-reaching 32-byte public-key vector")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("vectors", type=Path)
    parser.add_argument("--update", action="store_true")
    args = parser.parse_args()

    paths = sorted(
        path for path in args.vectors.glob("qrs_*.json") if not path.name.endswith(".schema.json")
    )
    if not paths:
        raise FixtureError(f"no qrs_*.json fixtures found under {args.vectors}")
    verified = [verify_one(path, args.update) for path in paths]
    verify_tapleaf_preimage_self_test(paths)
    crypto_reaching = [v["name"] for v in verified if v["failure_stage"] in {"none", "slh_dsa_verify"}]
    print(
        f"verified {len(verified)} provisional executable QRS structured fixtures "
        f"({len(crypto_reaching)} reach the modeled SLH-DSA verifier boundary; "
        "draft cryptographic vectors are checked separately by verify_qrs_vectors.py)"
    )
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (FixtureError, AssertionError) as exc:
        print(f"verify_qrs_fixtures.py: {exc}", file=sys.stderr)
        sys.exit(1)
