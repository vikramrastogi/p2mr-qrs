#!/usr/bin/env python3
"""Validate the consensus gap manifest without external dependencies."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "docs" / "evidence" / "consensus-gap-manifest.json"
SCHEMA = ROOT / "docs" / "evidence" / "consensus-gap-manifest.schema.json"
MARKDOWN = ROOT / "docs" / "evidence" / "consensus-gap-manifest.md"

REQUIRED_IDS = {
    "bip360_final_leaf_hashing",
    "bip360_final_branch_hashing",
    "bip360_final_future_leaf_behavior",
    "qrs_ext_flag_assignment",
    "final_sigmsg_definition",
    "bitcoin_core_validation_path_integration",
    "second_reviewed_slh_dsa_backend",
    "cross_hardware_reproduction",
    "reviewed_public_batch_schnorr_baseline",
    "wallet_descriptor_psbt_hardware_standards",
    "activation_parameters",
    "final_serialized_consensus_vectors",
}

ALLOWED_BLOCKS = {
    "activation",
    "complete_vectors",
    "draft_advancement",
    "resource_accounting_decision",
    "wallet_standards",
}

ALLOWED_OWNER_TYPES = {
    "future_bip360_review",
    "future_core_review",
    "future_implementation_review",
    "future_reviewer_reproduction",
    "future_wallet_standards",
    "future_activation_process",
    "future_batch_schnorr_review",
}

CRITICAL_GAP_SCHEMA_KEYS = {
    "id",
    "status",
    "blocks",
    "owner_type",
    "description",
    "evidence_required",
    "current_repo_artifact",
}


def fail(message: str) -> None:
    print(f"validate_consensus_gap_manifest.py: {message}", file=sys.stderr)
    raise SystemExit(1)


def load_json(path: Path) -> dict[str, Any]:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError:
        fail(f"missing file: {path.relative_to(ROOT)}")
    except json.JSONDecodeError as exc:
        fail(f"invalid JSON in {path.relative_to(ROOT)}: {exc}")


def require_string(item: dict[str, Any], key: str, gap_id: str) -> str:
    value = item.get(key)
    if not isinstance(value, str) or not value.strip():
        fail(f"{gap_id}: {key} must be a non-empty string")
    return value


def validate_artifact(path_text: str, gap_id: str) -> None:
    artifact = ROOT / path_text
    if not artifact.exists():
        fail(f"{gap_id}: current_repo_artifact does not exist: {path_text}")


def validate_schema_contract(schema: dict[str, Any]) -> None:
    if schema.get("title") != "P2MR QRS Consensus Gap Manifest":
        fail("schema title mismatch")

    gap_schema = schema.get("properties", {}).get("gaps", {}).get("items", {})
    required_keys = set(gap_schema.get("required", []))
    missing = sorted(CRITICAL_GAP_SCHEMA_KEYS - required_keys)
    if missing:
        fail("schema gap required keys missing: " + ", ".join(missing))

    schema_const = schema.get("properties", {}).get("schema", {}).get("const")
    if schema_const != "p2mr-qrs/consensus-gap-manifest/v0":
        fail("schema identifier contract mismatch")

    status_const = schema.get("properties", {}).get("status", {}).get("const")
    if status_const != "blocking_activation":
        fail("schema status contract mismatch")


def main() -> int:
    schema = load_json(SCHEMA)
    manifest = load_json(MANIFEST)

    validate_schema_contract(schema)
    if manifest.get("schema") != "p2mr-qrs/consensus-gap-manifest/v0":
        fail("manifest schema identifier mismatch")
    if manifest.get("status") != "blocking_activation":
        fail("manifest status must be blocking_activation")

    gaps = manifest.get("gaps")
    if not isinstance(gaps, list) or not gaps:
        fail("manifest gaps must be a non-empty array")

    seen: set[str] = set()
    for item in gaps:
        if not isinstance(item, dict):
            fail("each gap must be an object")
        gap_id = require_string(item, "id", "<unknown>")
        if gap_id in seen:
            fail(f"duplicate gap id: {gap_id}")
        seen.add(gap_id)

        if item.get("status") != "blocking_activation":
            fail(f"{gap_id}: status must be blocking_activation")
        owner_type = require_string(item, "owner_type", gap_id)
        if owner_type not in ALLOWED_OWNER_TYPES:
            fail(f"{gap_id}: unknown owner_type {owner_type}")

        blocks = item.get("blocks")
        if not isinstance(blocks, list) or not blocks:
            fail(f"{gap_id}: blocks must be a non-empty array")
        unknown_blocks = sorted(set(blocks) - ALLOWED_BLOCKS)
        if unknown_blocks:
            fail(f"{gap_id}: unknown blocks: {', '.join(unknown_blocks)}")
        if "activation" not in blocks:
            fail(f"{gap_id}: every listed gap must block activation")

        require_string(item, "description", gap_id)
        require_string(item, "evidence_required", gap_id)
        validate_artifact(require_string(item, "current_repo_artifact", gap_id), gap_id)

    missing = sorted(REQUIRED_IDS - seen)
    extra = sorted(seen - REQUIRED_IDS)
    if missing:
        fail("missing required gap ids: " + ", ".join(missing))
    if extra:
        fail("unexpected gap ids: " + ", ".join(extra))

    markdown = MARKDOWN.read_text(encoding="utf-8")
    missing_markdown = [gap_id for gap_id in sorted(REQUIRED_IDS) if gap_id not in markdown]
    if missing_markdown:
        fail("markdown missing gap ids: " + ", ".join(missing_markdown))

    print("consensus gap manifest valid")
    return 0


if __name__ == "__main__":
    sys.exit(main())
