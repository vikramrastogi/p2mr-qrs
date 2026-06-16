#!/usr/bin/env python3
"""Verify whether this package can wire a reviewed batch Schnorr baseline.

The script is intentionally conservative.  It does not infer a speedup from
individual verification timings and it treats any newly discovered public batch
API or upstream batch branch as a reason for human review.
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path


UPSTREAM_URL = "https://github.com/bitcoin-core/secp256k1.git"
UPSTREAM_PATTERNS = ("*batch*", "*schnorr*", "*bip340*")


def timeout_text(value: str | bytes | None) -> str:
    if value is None:
        return ""
    if isinstance(value, bytes):
        return value.decode("utf-8", errors="replace").strip()
    return value.strip()


def run_command(
    args: list[str],
    cwd: Path | None = None,
    timeout_seconds: float | None = None,
) -> tuple[int, str, str]:
    try:
        proc = subprocess.run(
            args,
            cwd=str(cwd) if cwd else None,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout_seconds,
            check=False,
        )
        return proc.returncode, proc.stdout.strip(), proc.stderr.strip()
    except subprocess.TimeoutExpired as exc:
        return 124, timeout_text(exc.stdout), timeout_text(exc.stderr)


def display_path(path: Path, root: Path) -> str:
    try:
        return str(path.relative_to(root))
    except ValueError:
        return str(path)


def matching_lines(path: Path, pattern: str, root: Path) -> list[dict[str, object]]:
    rx = re.compile(pattern, re.IGNORECASE)
    out: list[dict[str, object]] = []
    if not path.exists():
        return out
    for number, line in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
        if rx.search(line):
            out.append({"path": display_path(path, root), "line": number, "text": line.strip()})
    return out


def write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--json", type=Path, default=None)
    parser.add_argument("--markdown", type=Path, default=None)
    parser.add_argument("--skip-upstream", action="store_true")
    parser.add_argument("--upstream-timeout-seconds", type=float, default=15.0)
    args = parser.parse_args()

    root = args.root.resolve()
    secp_dir = root / "third_party" / "secp256k1"
    commit_file = root / "third_party" / "secp256k1.COMMIT"
    header = secp_dir / "include" / "secp256k1_schnorrsig.h"
    tests = secp_dir / "src" / "modules" / "schnorrsig" / "tests_impl.h"

    expected_commit = commit_file.read_text(encoding="utf-8").strip() if commit_file.exists() else ""
    if (secp_dir / ".git").exists():
        rc, actual_commit, actual_commit_err = run_command(["git", "rev-parse", "HEAD"], cwd=secp_dir)
        if rc != 0:
            actual_commit = f"unavailable: {actual_commit_err}"
    else:
        actual_commit = (
            "vendored source has no .git metadata; expected commit recorded in "
            "third_party/secp256k1.COMMIT"
        )

    header_text = header.read_text(encoding="utf-8") if header.exists() else ""
    public_api_names = sorted(set(re.findall(r"\bsecp256k1_schnorrsig_[A-Za-z0-9_]+\b", header_text)))
    public_batch_mentions = matching_lines(header, r"batch|verify_batch", root)
    public_batch_api = any("batch" in name.lower() for name in public_api_names) or bool(public_batch_mentions)
    verify_batch_todo = matching_lines(tests, r"verify_batch \(TODO\)", root)

    upstream = {
        "url": UPSTREAM_URL,
        "patterns": list(UPSTREAM_PATTERNS),
        "status": "skipped" if args.skip_upstream else "not_run",
        "timeout_seconds": args.upstream_timeout_seconds,
        "matching_heads": [],
        "stderr": "",
    }
    if not args.skip_upstream:
        command = ["git", "ls-remote", "--heads", UPSTREAM_URL, *UPSTREAM_PATTERNS]
        rc, stdout, stderr = run_command(command, timeout_seconds=args.upstream_timeout_seconds)
        if rc == 0:
            upstream["status"] = "ok"
        elif rc == 124:
            upstream["status"] = "timeout"
            stderr = stderr or f"git ls-remote exceeded {args.upstream_timeout_seconds} seconds"
        else:
            upstream["status"] = "error"
        upstream["stderr"] = stderr
        upstream["matching_heads"] = [line for line in stdout.splitlines() if line.strip()]

    upstream_checked = upstream["status"] in {"ok", "skipped"}
    upstream_has_candidate = upstream["status"] == "ok" and bool(upstream["matching_heads"])
    if public_batch_api or upstream_has_candidate:
        status = "review-needed"
    elif upstream_checked:
        status = "unavailable"
    else:
        status = "evidence-incomplete"

    if status == "unavailable" and upstream["status"] == "ok":
        reason = (
            f"Bundled upstream bitcoin-core/secp256k1 commit {expected_commit} exposes no "
            "public BIP-340 batch verification API; local schnorrsig tests still mark "
            "verify_batch as TODO; and the upstream head search found no batch/Schnorr/BIP-340 "
            "branch to wire as a reviewed experimental baseline."
        )
    elif status == "unavailable":
        reason = (
            f"Bundled upstream bitcoin-core/secp256k1 commit {expected_commit} exposes no "
            "public BIP-340 batch verification API, and local schnorrsig tests still mark "
            "verify_batch as TODO. The upstream head search was skipped by caller request; "
            "run without --skip-upstream before treating this as network-checked evidence."
        )
    else:
        if status == "review-needed":
            reason = (
                "A possible local or upstream batch candidate was found. Review it before keeping "
                "the batch baseline unavailable."
            )
        else:
            reason = (
                "The upstream batch/Schnorr/BIP-340 head search did not complete, so the batch "
                "baseline evidence is incomplete. This does not establish that no reviewed batch "
                "baseline exists."
            )

    result = {
        "schema": "qrs-native-bench/batch-baseline-evidence/v0",
        "status": status,
        "reason": reason,
        "policy": "No batch Schnorr timing is synthesized from assumed speedups.",
        "bundled_commit_expected": expected_commit,
        "bundled_commit_actual": actual_commit,
        "public_schnorr_header": display_path(header, root),
        "public_schnorr_api_names": public_api_names,
        "public_batch_api_detected": public_batch_api,
        "public_batch_mentions": public_batch_mentions,
        "verify_batch_todo_mentions": verify_batch_todo,
        "upstream_head_search": upstream,
    }

    md_lines = [
        "# Batch Schnorr Baseline Evidence",
        "",
        f"- `status`: {status}",
        f"- `reason`: {reason}",
        f"- `bundled_commit_expected`: {expected_commit}",
        f"- `bundled_commit_actual`: {actual_commit}",
        f"- `public_batch_api_detected`: {str(public_batch_api).lower()}",
        f"- `verify_batch_todo_mentions`: {len(verify_batch_todo)}",
        f"- `upstream_head_search.status`: {upstream['status']}",
        f"- `upstream_head_search.timeout_seconds`: {upstream['timeout_seconds']}",
        f"- `upstream_head_search.matching_heads`: {len(upstream['matching_heads'])}",
        "- `policy`: No batch Schnorr timing is synthesized from assumed speedups.",
        "",
        "## Public Schnorr APIs",
        "",
    ]
    md_lines.extend(f"- `{name}`" for name in public_api_names)
    md_lines.append("")
    md_lines.append("## verify_batch TODO Mentions")
    md_lines.append("")
    if verify_batch_todo:
        for item in verify_batch_todo:
            md_lines.append(f"- `{item['path']}:{item['line']}`: {item['text']}")
    else:
        md_lines.append("- none")
    md_lines.append("")
    md_lines.append("## Upstream Matching Heads")
    md_lines.append("")
    if upstream["matching_heads"]:
        md_lines.extend(f"- `{head}`" for head in upstream["matching_heads"])
    else:
        md_lines.append("- none")
    md_lines.append("")

    if args.json:
        write_text(args.json, json.dumps(result, indent=2, sort_keys=True) + "\n")
    if args.markdown:
        write_text(args.markdown, "\n".join(md_lines))

    print(reason)
    return 0 if status == "unavailable" else 2


if __name__ == "__main__":
    sys.exit(main())
