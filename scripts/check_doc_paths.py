#!/usr/bin/env python3
"""Fail on stale public documentation paths after the docs reorg."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MOVE_MAP = Path("docs/internal/move-map.md")

REQUIRED_PATHS = [
    "README.md",
    "docs/README.md",
    "docs/review/start-here.md",
    "docs/spec/bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki",
    "docs/evidence/consensus-gap-manifest.json",
    "docs/reproducibility/reproducibility.md",
]


def fail(message: str) -> None:
    print(f"check_doc_paths.py: {message}", file=sys.stderr)
    raise SystemExit(1)


def tracked_files() -> list[Path]:
    result = subprocess.run(
        ["git", "-C", str(ROOT), "ls-files", "-z"],
        check=True,
        stdout=subprocess.PIPE,
    )
    return [
        Path(part.decode("utf-8"))
        for part in result.stdout.split(b"\0")
        if part
    ]


def removed_paths_from_move_map() -> list[str]:
    paths: list[str] = []
    for line in (ROOT / MOVE_MAP).read_text(encoding="utf-8").splitlines():
        stripped = line.strip()
        if not stripped.startswith("| `"):
            continue
        first_cell = stripped.split("|", 2)[1].strip()
        if first_cell.startswith("`") and first_cell.endswith("`"):
            paths.append(first_cell[1:-1])
    if not paths:
        fail(f"no old paths found in {MOVE_MAP}")
    return paths


def check_required_paths() -> None:
    for path in REQUIRED_PATHS:
        if not (ROOT / path).exists():
            fail(f"required path missing: {path}")


def check_removed_paths_absent() -> None:
    for path in removed_paths_from_move_map():
        if (ROOT / path).exists():
            fail(f"removed path still exists: {path}")

    root_numbered = sorted(ROOT.glob("0[0-9]_*.md"))
    if root_numbered:
        fail("root numbered implementation notes still exist: " + ", ".join(p.name for p in root_numbered))


def check_stale_mentions() -> None:
    failures: list[str] = []
    stale_needles = removed_paths_from_move_map()
    doubled_topics = [f"docs/{topic}/" + "docs/" for topic in (
        "review",
        "spec",
        "evidence",
        "reproducibility",
        "internal",
    )]
    for relpath in tracked_files():
        if relpath == MOVE_MAP:
            continue
        if relpath.parts and relpath.parts[0] in {".git", "build", "third_party"}:
            continue
        path = ROOT / relpath
        try:
            text = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            continue
        for lineno, line in enumerate(text.splitlines(), start=1):
            if any(needle in line for needle in stale_needles) or any(
                doubled in line for doubled in doubled_topics
            ):
                failures.append(f"{relpath}:{lineno}: {line}")
    if failures:
        print("\n".join(failures), file=sys.stderr)
        fail("stale documentation path mention found")


def main() -> int:
    check_required_paths()
    check_removed_paths_absent()
    check_stale_mentions()
    print("documentation paths valid")
    return 0


if __name__ == "__main__":
    sys.exit(main())
