#!/usr/bin/env python3
"""Check Python/native agreement for the provisional QRS digest model."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

from compute_qrs_digest_model import compute_vector_digest, load_vectors  # type: ignore


class AgreementError(AssertionError):
    pass


def native_digest(binary: Path, vector: Path) -> dict:
    proc = subprocess.run(
        [str(binary), "--qrs-digest-vector", str(vector)],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    if proc.returncode != 0:
        raise AgreementError(f"{vector}: native digest failed: {proc.stderr.strip()}")
    return json.loads(proc.stdout)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("vectors", type=Path)
    parser.add_argument("--binary", type=Path, default=Path("build/qrs_native_bench"))
    args = parser.parse_args()

    vectors = load_vectors(args.vectors)
    for path, data in vectors:
        py = compute_vector_digest(data, path)
        native = native_digest(args.binary, path)
        for field in ["leaf_hash", "merkle_root", "qrs_msg"]:
            if native[field] != py[field]:
                raise AgreementError(
                    f"{path}: {field} mismatch: python={py[field]} native={native[field]}"
                )
    print(f"cross-implementation QRS digest agreement passed for {len(vectors)} vectors")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (AgreementError, AssertionError, KeyError, json.JSONDecodeError) as exc:
        print(f"check_qrs_digest_agreement.py: {exc}", file=sys.stderr)
        sys.exit(1)
