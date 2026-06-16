#!/usr/bin/env python3
"""Validate the provisional QRS vector coverage matrix."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
MATRIX = ROOT / "test_vectors" / "vector_coverage_matrix.json"
SCHEMA = ROOT / "test_vectors" / "vector_coverage_matrix.schema.json"

REQUIRED_CASE_IDS = {
    "valid_no_annex",
    "valid_with_annex",
    "invalid_fixed_length_signature",
    "invalid_public_key_length",
    "invalid_signature_length",
    "invalid_control_block_length",
    "control_low_bit_unset",
    "invalid_leaf_version",
    "merkle_root_mismatch",
    "malformed_witness_missing_element",
    "malformed_witness_extra_element",
    "malformed_witness_extra_after_annex",
    "maximum_depth_merkle_path",
    "multi_input_distinct_spent_outputs",
    "annex_changes_qrs_msg",
    "output_amount_changes_qrs_msg",
    "spent_output_amount_changes_qrs_msg",
    "spent_output_script_changes_qrs_msg",
    "leaf_pubkey_changes_qrs_msg",
    "wrong_message_signature_fails",
    "wrong_public_key_signature_fails",
}

ARTIFACT_TYPES = {
    "fixture",
    "fuzz_case",
    "negative_case",
    "digest_self_test",
    "benchmark_invalid_case",
}

REQUIRED_TOP_LEVEL_SCHEMA_KEYS = {
    "schema",
    "status",
    "description",
    "rows",
}

REQUIRED_ROW_SCHEMA_KEYS = {
    "case_id",
    "artifact_type",
    "artifact",
    "expected_result",
    "failure_stage",
    "reaches_slh_dsa_verifier",
    "required_by",
}


def fail(message: str) -> None:
    print(f"validate_vector_coverage_matrix.py: {message}", file=sys.stderr)
    raise SystemExit(1)


def load_json(path: Path) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError:
        fail(f"missing file: {path.relative_to(ROOT)}")
    except json.JSONDecodeError as exc:
        fail(f"invalid JSON in {path.relative_to(ROOT)}: {exc}")


def require(value: bool, message: str) -> None:
    if not value:
        fail(message)


def validate_fixture(row: dict[str, Any], artifact: Path) -> None:
    data = load_json(artifact)
    require(data.get("expected_result") == row["expected_result"], f"{row['case_id']}: expected_result mismatch")
    require(data.get("failure_stage") == row["failure_stage"], f"{row['case_id']}: failure_stage mismatch")
    require(
        data.get("reaches_slh_dsa_verifier") == row["reaches_slh_dsa_verifier"],
        f"{row['case_id']}: reaches_slh_dsa_verifier mismatch",
    )


def validate_case_metadata(row: dict[str, Any], artifact: Path) -> None:
    data = load_json(artifact)
    stage = data.get("expected_failure_stage")
    if "cases" in data:
        case_name = row.get("artifact_case")
        require(isinstance(case_name, str) and case_name, f"{row['case_id']}: artifact_case required")
        matches = [case for case in data["cases"] if case.get("name") == case_name]
        require(len(matches) == 1, f"{row['case_id']}: negative case {case_name!r} not found")
        data = matches[0]
        stage = data.get("expected_failure_stage")
    require(stage == row["failure_stage"], f"{row['case_id']}: failure_stage mismatch")
    require(
        data.get("reaches_slh_dsa_verifier") == row["reaches_slh_dsa_verifier"],
        f"{row['case_id']}: reaches_slh_dsa_verifier mismatch",
    )
    require(row["expected_result"] is False, f"{row['case_id']}: negative/fuzz rows must expect false")


def validate_source_label(row: dict[str, Any], artifact: Path) -> None:
    label = row.get("self_test_label")
    require(isinstance(label, str) and label, f"{row['case_id']}: self_test_label required")
    text = artifact.read_text(encoding="utf-8")
    require(label in text, f"{row['case_id']}: label {label!r} not found in {artifact.relative_to(ROOT)}")


def validate_schema_contract(schema: dict[str, Any]) -> None:
    require(schema.get("title") == "P2MR QRS Vector Coverage Matrix", "schema title mismatch")

    top_required = set(schema.get("required", []))
    missing_top = sorted(REQUIRED_TOP_LEVEL_SCHEMA_KEYS - top_required)
    require(not missing_top, "schema top-level required keys missing: " + ", ".join(missing_top))

    row_schema = schema.get("properties", {}).get("rows", {}).get("items", {})
    row_required = set(row_schema.get("required", []))
    missing_row = sorted(REQUIRED_ROW_SCHEMA_KEYS - row_required)
    require(not missing_row, "schema row required keys missing: " + ", ".join(missing_row))

    schema_const = schema.get("properties", {}).get("schema", {}).get("const")
    require(schema_const == "p2mr-qrs/vector-coverage-matrix/v0", "schema identifier contract mismatch")
    status_const = schema.get("properties", {}).get("status", {}).get("const")
    require(
        status_const == "provisional_executable_coverage_not_final_consensus_vectors",
        "schema status contract mismatch",
    )


def main() -> int:
    schema = load_json(SCHEMA)
    matrix = load_json(MATRIX)
    validate_schema_contract(schema)
    require(matrix.get("schema") == "p2mr-qrs/vector-coverage-matrix/v0", "unexpected matrix schema")
    require(
        matrix.get("status") == "provisional_executable_coverage_not_final_consensus_vectors",
        "unexpected matrix status",
    )
    rows = matrix.get("rows")
    require(isinstance(rows, list) and rows, "matrix rows must be a non-empty array")

    seen: set[str] = set()
    for row in rows:
        require(isinstance(row, dict), "each matrix row must be an object")
        case_id = row.get("case_id")
        require(isinstance(case_id, str) and case_id, "case_id is required")
        require(case_id not in seen, f"duplicate case_id {case_id}")
        seen.add(case_id)
        artifact_type = row.get("artifact_type")
        require(artifact_type in ARTIFACT_TYPES, f"{case_id}: unknown artifact_type {artifact_type!r}")
        if "self_test_label" in row:
            require(
                artifact_type in {"digest_self_test", "benchmark_invalid_case"},
                f"{case_id}: self_test_label is only valid for digest_self_test and benchmark_invalid_case",
            )
        if "artifact_case" in row:
            require(artifact_type == "negative_case", f"{case_id}: artifact_case is only valid for negative_case rows")
        artifact_text = row.get("artifact")
        require(isinstance(artifact_text, str) and artifact_text, f"{case_id}: artifact is required")
        artifact = ROOT / artifact_text
        require(artifact.exists(), f"{case_id}: artifact does not exist: {artifact_text}")
        require(isinstance(row.get("expected_result"), bool), f"{case_id}: expected_result must be boolean")
        require(isinstance(row.get("failure_stage"), str) and row["failure_stage"], f"{case_id}: failure_stage required")
        require(
            isinstance(row.get("reaches_slh_dsa_verifier"), bool),
            f"{case_id}: reaches_slh_dsa_verifier must be boolean",
        )
        require(isinstance(row.get("required_by"), list) and row["required_by"], f"{case_id}: required_by required")

        if artifact_type == "fixture":
            validate_fixture(row, artifact)
        elif artifact_type in {"fuzz_case", "negative_case"}:
            validate_case_metadata(row, artifact)
        elif artifact_type in {"digest_self_test", "benchmark_invalid_case"}:
            validate_source_label(row, artifact)

    missing = sorted(REQUIRED_CASE_IDS - seen)
    extra = sorted(seen - REQUIRED_CASE_IDS)
    if missing:
        fail("missing required case ids: " + ", ".join(missing))
    if extra:
        fail("unexpected case ids: " + ", ".join(extra))

    print(f"validated {len(rows)} vector coverage matrix rows")
    return 0


if __name__ == "__main__":
    main()
