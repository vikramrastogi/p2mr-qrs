#!/usr/bin/env python3
"""Fail if public docs drift back to stale pre-reorg paths."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DOCS = "docs/"
MOVE_MAP = Path("docs/internal/move-map.md")


def tracked_files() -> list[Path]:
    out = subprocess.check_output(["git", "-C", str(ROOT), "ls-files"], text=True)
    return [Path(line) for line in out.splitlines() if line]


def fail(message: str) -> None:
    print(f"check_doc_paths.py: {message}", file=sys.stderr)
    raise SystemExit(1)


def read_text(path: Path) -> str:
    try:
        return (ROOT / path).read_text(encoding="utf-8")
    except UnicodeDecodeError:
        return ""


def doc(name: str) -> str:
    return DOCS + name


STALE_DOC_PATHS = [
    doc(name)
    for name in [
        "REVIEW_THIS_FIRST.md",
        "REVIEWER_DOSSIER.md",
        "PUBLIC_REVIEW_READINESS.md",
        "PUBLIC_REVIEW_POST_DRAFT.md",
        "RESOURCE_ACCOUNTING_DECISION.md",
        "EXPLICIT_QRS_BUDGET_FALLBACK.md",
        "BATCH_SCHNORR_BASELINE_STATUS.md",
        "BITCOIN_CORE_INTEGRATION_REQUIREMENTS.md",
        "SLH_DSA_VERIFY_COST_ANALYSIS.md",
        "VECTOR_COVERAGE_MATRIX.md",
        "CONSENSUS_GAP_MANIFEST.md",
        "REPRODUCIBILITY.md",
        "REPRODUCTION_MATRIX.md",
        "FINAL_TRANSACTION_VECTOR_SCHEMA.md",
        "BIP360_DEPENDENCY_MATRIX.md",
        "RELEASE_CHECKLIST.md",
        "native-benchmark-report-template.md",
        "bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki",
        "core-validation-model.md",
        "consensus-gap-manifest.json",
        "consensus-gap-manifest.schema.json",
    ]
]

ROOT_NUMBERED_DOCS = [
    f"{prefix}_{suffix}"
    for prefix, suffix in [
        ("00", "README.md"),
        ("01", "native_benchmark_harness.md"),
        ("02", "openssl_slh_dsa_backend.md"),
        ("03", "libsecp256k1_schnorr_backend.md"),
        ("04", "batch_schnorr_baseline.md"),
        ("05", "block_validation_model.md"),
        ("06", "report_generation.md"),
        ("07", "reviewer_validation.md"),
    ]
]

REQUIRED_TRACKED = [
    "README.md",
    "docs/README.md",
    "docs/internal/move-map.md",
    "docs/review/start-here.md",
    "docs/spec/bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki",
    "docs/spec/bip360-dependency-matrix.md",
    "docs/evidence/consensus-gap-manifest.json",
    "docs/reproducibility/reproducibility.md",
    "scripts/check_doc_paths.py",
]

README_REQUIRED = [
    "Start here: docs/review/start-here.md",
    "Canonical BIP draft: docs/spec/bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki",
    "BIP-360 dependency matrix: docs/spec/bip360-dependency-matrix.md",
    "Full documentation map: docs/README.md",
    "Publication gate: `bash scripts/release_check.sh`",
]


def main() -> None:
    files = tracked_files()
    tracked = {path.as_posix() for path in files}

    for required in REQUIRED_TRACKED:
        if not (ROOT / required).is_file():
            fail(f"required canonical path is missing: {required}")

    for stale in STALE_DOC_PATHS + ROOT_NUMBERED_DOCS:
        if stale in tracked:
            fail(f"stale public doc path is still tracked: {stale}")

    readme = read_text(Path("README.md"))
    for required in README_REQUIRED:
        if required not in readme:
            fail(f"README.md missing required front-door link: {required}")
    if "out/full.md" in readme:
        fail("README.md must not put out/full.md on the front door")

    scan_skip_prefixes = (
        ".git/",
        "build/",
        "third_party/",
    )
    allowed_stale_mentions = {MOVE_MAP.as_posix()}
    stale_needles = STALE_DOC_PATHS + ROOT_NUMBERED_DOCS
    for path in files:
        posix = path.as_posix()
        if posix in allowed_stale_mentions:
            continue
        if posix.startswith(scan_skip_prefixes):
            continue
        text = read_text(path)
        for needle in stale_needles:
            if needle in text:
                fail(f"stale path mention outside move map: {posix}: {needle}")

    print("documentation path guard passed")


if __name__ == "__main__":
    main()
