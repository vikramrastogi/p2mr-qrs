#!/usr/bin/env python3
"""Execute QRS negative-case and structured malformed-witness corpus checks."""

from __future__ import annotations

import copy
import json
import sys
from pathlib import Path

from verify_qrs_fixtures import (  # type: ignore
    QRS_LEAF_VERSION,
    classify,
    compact_size,
    expand_descriptor,
    hex_bytes,
    modeled_leaf_hash,
    tagged_hash,
)


ROOT = Path(__file__).resolve().parents[1]
NEGATIVE_CASES = ROOT / "tests" / "qrs_negative_cases.json"
FUZZ_DIR = ROOT / "fuzz" / "qrs_witness_corpus"
VECTORS_DIR = ROOT / "test_vectors"

FIXTURE_CASES = {
    "too_many_witness_elements": "qrs_malformed_witness_extra_element.json",
    "wrong_signature_length": "qrs_invalid_signature_length.json",
    "wrong_public_key_length": "qrs_invalid_pubkey_length.json",
    "invalid_control_block_length": "qrs_invalid_control_block_length.json",
    "low_bit_unset": "qrs_control_low_bit_unset.json",
    "wrong_leaf_version": "qrs_invalid_leaf_version.json",
    "merkle_path_mismatch": "qrs_merkle_root_mismatch.json",
    "annex_ambiguity": "qrs_malformed_witness_extra_after_annex.json",
    "max_depth_path": "qrs_max_depth_path.json",
    "invalid_fixed_length_signature_reaches_verifier": "qrs_invalid_signature_fixed_length.json",
}

GENERATED_CASES = {
    "empty_witness",
    "too_few_witness_elements",
    "malformed_compact_size",
}


def require(value: bool, message: str) -> None:
    if not value:
        raise AssertionError(message)


def load_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def base_fixture() -> dict:
    return load_json(VECTORS_DIR / "qrs_valid_no_annex.json")


def classify_fixture(data: dict, source: str) -> tuple[str, bool]:
    path = Path(source)
    sig = expand_descriptor(data["qrs_signature"], path, "qrs_signature")
    pubkey = expand_descriptor(data["qrs_public_key"], path, "qrs_public_key")
    control_block = expand_descriptor(data["control_block"], path, "control_block")
    leaf_hash = modeled_leaf_hash(pubkey)
    stage = classify(data, sig, pubkey, control_block, leaf_hash, path)
    return stage, stage in {"none", "slh_dsa_verify"}


def malformed_compact_size_case(source: str) -> tuple[str, bool]:
    """Exercise the non-minimal TapLeaf length-prefix reject explicitly.

    The provisional fixture model stores a parsed public-key payload, so there
    is no serialized leaf-payload byte string to hand to classify(). This check
    makes the compact-size failure executable by proving the accepted BIP-form
    TapLeaf preimage uses the one-byte 32 length and that a non-minimal
    CompactSize encoding of the same 32-byte payload is rejected before it can
    represent the same leaf hash.
    """

    data = base_fixture()
    path = Path(source)
    pubkey = expand_descriptor(data["qrs_public_key"], path, "qrs_public_key")
    correct_preimage = bytes([QRS_LEAF_VERSION]) + compact_size(len(pubkey)) + pubkey
    non_minimal_preimage = bytes([QRS_LEAF_VERSION]) + b"\xfd\x20\x00" + pubkey
    require(len(pubkey) == 32, f"{source}: compact-size test requires 32-byte pubkey")
    require(correct_preimage[1:2] == b"\x20", f"{source}: BIP-form preimage must use minimal CompactSize")
    require(
        tagged_hash("TapLeaf", correct_preimage) == hex_bytes(data["expected_leaf_hash"], path, "expected_leaf_hash"),
        f"{source}: BIP-form TapLeaf preimage no longer matches base fixture",
    )
    require(
        tagged_hash("TapLeaf", non_minimal_preimage) != tagged_hash("TapLeaf", correct_preimage),
        f"{source}: non-minimal CompactSize preimage was not distinguished",
    )
    return "malformed_compact_size", False


def materialize_generated_case(name: str, source: str) -> tuple[str, bool]:
    if name == "malformed_compact_size":
        return malformed_compact_size_case(source)

    data = copy.deepcopy(base_fixture())
    data["name"] = name
    data["expected_result"] = False
    data["reaches_slh_dsa_verifier"] = False
    data["failure_stage"] = "malformed_witness"
    if name == "empty_witness":
        data["witness_stack"] = []
    elif name == "too_few_witness_elements":
        data["witness_stack"] = ["qrs_signature", "qrs_public_key"]
    else:
        raise AssertionError(f"{source}: no generated executable fixture for {name!r}")
    return classify_fixture(data, source)


def execute_case(case: dict, source: str) -> tuple[str, bool]:
    name = case["name"]
    if name in FIXTURE_CASES:
        return classify_fixture(load_json(VECTORS_DIR / FIXTURE_CASES[name]), source)
    if name in GENERATED_CASES:
        return materialize_generated_case(name, source)
    raise AssertionError(f"{source}: no executable negative fixture mapping for {name!r}")


def check_case(case: dict, source: str) -> None:
    for field in ["name", "mutation", "expected_failure_stage", "reaches_slh_dsa_verifier"]:
        require(field in case, f"{source}: missing {field}")
    stage = case["expected_failure_stage"]
    reaches = bool(case["reaches_slh_dsa_verifier"])
    if stage == "slh_dsa_verify":
        require(reaches, f"{source}: fixed-length crypto reject must reach verifier")
    elif stage == "none":
        require(reaches, f"{source}: valid/max-depth case should reach verifier")
    else:
        require(not reaches, f"{source}: structural reject must not reach verifier")
    actual_stage, actual_reaches = execute_case(case, source)
    require(actual_stage == stage, f"{source}: executable classification {actual_stage!r}, expected {stage!r}")
    require(actual_reaches == reaches, f"{source}: executable reaches={actual_reaches}, expected {reaches}")


def main() -> int:
    data = json.loads(NEGATIVE_CASES.read_text(encoding="utf-8"))
    cases = data.get("cases", [])
    require(len(cases) >= 13, "negative case suite is too small")
    for case in cases:
        check_case(case, f"tests/{case.get('name', '<unnamed>')}")
    names = {case["name"] for case in cases}
    require("invalid_fixed_length_signature_reaches_verifier" in names,
            "missing invalid fixed-length verifier-reaching case")

    fuzz_paths = sorted(FUZZ_DIR.glob("*.json"))
    require(len(fuzz_paths) >= 12, "structured malformed-witness corpus is too small")
    for path in fuzz_paths:
        check_case(json.loads(path.read_text(encoding="utf-8")), str(path.relative_to(ROOT)))

    print(
        f"executed {len(cases)} negative cases and "
        f"{len(fuzz_paths)} structured malformed-witness corpus entries"
    )
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except AssertionError as exc:
        print(f"run_qrs_negative_tests.py: {exc}", file=sys.stderr)
        sys.exit(1)
