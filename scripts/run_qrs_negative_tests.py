#!/usr/bin/env python3
"""Validate QRS negative-case and fuzz-corpus classification."""

from __future__ import annotations

import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
NEGATIVE_CASES = ROOT / "tests" / "qrs_negative_cases.json"
FUZZ_DIR = ROOT / "fuzz" / "qrs_witness_corpus"


def require(value: bool, message: str) -> None:
    if not value:
        raise AssertionError(message)


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
    require(len(fuzz_paths) >= 12, "fuzz corpus is too small")
    for path in fuzz_paths:
        check_case(json.loads(path.read_text(encoding="utf-8")), str(path.relative_to(ROOT)))

    print(f"validated {len(cases)} negative cases and {len(fuzz_paths)} fuzz corpus entries")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except AssertionError as exc:
        print(f"run_qrs_negative_tests.py: {exc}", file=sys.stderr)
        sys.exit(1)
