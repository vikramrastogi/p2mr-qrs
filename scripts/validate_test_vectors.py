#!/usr/bin/env python3
"""Validate QRS provisional structured-fixture metadata."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path


REQUIRED_FIELDS = {
    "name",
    "transaction",
    "spent_output_scriptPubKey",
    "witness_stack",
    "qrs_public_key",
    "qrs_signature",
    "control_block",
    "expected_leaf_hash",
    "expected_qrs_msg",
    "expected_result",
    "failure_stage",
    "reaches_slh_dsa_verifier",
}

STRUCTURAL_FAILURES = {
    "signature_length",
    "public_key_length",
    "control_block_length",
    "control_low_bit",
    "leaf_version",
    "merkle_root_mismatch",
    "malformed_witness",
}


def fail(path: Path, message: str) -> None:
    raise AssertionError(f"{path}: {message}")


def hex32(value: str) -> bool:
    return isinstance(value, str) and re.fullmatch(r"[0-9a-f]{64}", value) is not None


def descriptor_len(value: object, field: str, path: Path) -> int:
    if isinstance(value, str):
      if not re.fullmatch(r"[0-9a-f]*", value) or len(value) % 2:
          fail(path, f"{field} must be even-length lowercase hex")
      return len(value) // 2
    if not isinstance(value, dict):
        fail(path, f"{field} must be hex or descriptor object")
    if "bytes" in value:
        return int(value["bytes"])
    if "hex" in value:
        h = value["hex"]
        if not re.fullmatch(r"[0-9a-f]*", h) or len(h) % 2:
            fail(path, f"{field}.hex must be even-length lowercase hex")
        return len(h) // 2
    fail(path, f"{field} descriptor must include bytes or hex")
    return 0


def validate_vector(path: Path) -> dict:
    data = json.loads(path.read_text(encoding="utf-8"))
    missing = REQUIRED_FIELDS - set(data)
    if missing:
        fail(path, f"missing required fields: {sorted(missing)}")
    if data["name"] != path.stem:
        fail(path, "name must match filename")
    if not isinstance(data["transaction"], dict):
        fail(path, "transaction must use structured fields")
    if not isinstance(data["witness_stack"], list):
        fail(path, "witness_stack must be a list")
    if not hex32(data["expected_leaf_hash"]):
        fail(path, "expected_leaf_hash must be 32-byte lowercase hex")
    if not hex32(data["expected_qrs_msg"]):
        fail(path, "expected_qrs_msg must be 32-byte lowercase hex")

    pub_len = descriptor_len(data["qrs_public_key"], "qrs_public_key", path)
    sig_len = descriptor_len(data["qrs_signature"], "qrs_signature", path)
    control_len = descriptor_len(data["control_block"], "control_block", path)

    expected = bool(data["expected_result"])
    stage = data["failure_stage"]
    reaches = bool(data["reaches_slh_dsa_verifier"])

    if expected:
        if stage != "none" or not reaches:
            fail(path, "valid vectors must have failure_stage none and reach verifier")
    else:
        if stage == "none":
            fail(path, "invalid vectors must name a failure_stage")
        if stage == "slh_dsa_verify":
            if sig_len != 7856 or not reaches:
                fail(path, "fixed-length crypto reject must be 7856 bytes and reach verifier")
        elif stage in STRUCTURAL_FAILURES:
            if reaches:
                fail(path, "structural reject must not reach verifier")
        else:
            fail(path, f"unknown failure_stage {stage!r}")

    if path.name == "qrs_invalid_signature_fixed_length.json":
        if stage != "slh_dsa_verify" or sig_len != 7856 or not reaches:
            fail(path, "fixed-length invalid signature vector invariant failed")
    if path.name == "qrs_invalid_signature_length.json" and sig_len == 7856:
        fail(path, "invalid signature length vector must not use 7856-byte signature")
    if path.name == "qrs_invalid_pubkey_length.json" and pub_len == 32:
        fail(path, "invalid pubkey length vector must not use 32-byte pubkey")
    if path.name == "qrs_invalid_control_block_length.json" and (control_len - 1) % 32 == 0:
        fail(path, "invalid control-block length vector must not be 1 + 32*m bytes")

    return data


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("vectors", type=Path)
    args = parser.parse_args()

    paths = sorted(
        path for path in args.vectors.glob("qrs_*.json") if not path.name.endswith(".schema.json")
    )
    if len(paths) < 10:
        raise AssertionError("expected at least 10 QRS vectors")
    vectors = [validate_vector(path) for path in paths]
    names = {v["name"] for v in vectors}
    required = {
        "qrs_valid_no_annex",
        "qrs_valid_with_annex",
        "qrs_invalid_signature_fixed_length",
        "qrs_invalid_signature_length",
        "qrs_invalid_pubkey_length",
        "qrs_invalid_control_block_length",
        "qrs_control_low_bit_unset",
        "qrs_invalid_leaf_version",
        "qrs_merkle_root_mismatch",
        "qrs_max_depth_path",
    }
    missing = required - names
    if missing:
        raise AssertionError(f"missing required vector names: {sorted(missing)}")
    print(f"validated {len(vectors)} QRS structured fixture vectors")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except AssertionError as exc:
        print(f"validate_test_vectors.py: {exc}", file=sys.stderr)
        sys.exit(1)
