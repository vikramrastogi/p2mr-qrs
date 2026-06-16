#!/usr/bin/env python3
"""Turn benchmark reports into an explicit resource-accounting decision."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any


class DecisionError(AssertionError):
    pass


def require(value: bool, message: str) -> None:
    if not value:
        raise DecisionError(message)


def nested(data: dict[str, Any], path: str) -> Any:
    cur: Any = data
    for part in path.split("."):
        cur = cur[part]
    return cur


def status(data: dict[str, Any], path: str) -> str:
    return str(nested(data, path).get("status", "missing"))


def p99_ms(data: dict[str, Any], path: str) -> float:
    value = nested(data, path).get("p99_ms")
    require(value is not None, f"{path}.p99_ms is unavailable")
    return float(value)


def add_check(checks: list[dict[str, Any]], name: str, passed: bool, detail: str) -> None:
    checks.append({"name": name, "passed": passed, "detail": detail})


def write_text(path: Path | None, text: str) -> None:
    if path is None:
        return
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def render_markdown(decision: dict[str, Any]) -> str:
    lines = [
        "# Resource Accounting Evaluation",
        "",
        "## Pass/Fail Conclusion",
        "",
        f"- `draft_rule_status`: {decision['draft_rule_status']}",
        f"- `activation_ready`: {str(decision['activation_ready']).lower()}",
        f"- `explicit_qrs_budget_required`: {str(decision['explicit_qrs_budget_required']).lower()}",
        f"- `fallback_budget_status`: {decision['fallback_budget_status']}",
        f"- `conclusion`: {decision['conclusion']}",
        "",
        "## Compared Estimates",
        "",
        "| Estimate | p99 ms |",
        "| --- | ---: |",
    ]
    for name, value in decision["p99_ms"].items():
        if value is None:
            lines.append(f"| {name} | unavailable |")
        else:
            lines.append(f"| {name} | {value:.3f} |")
    lines.extend(["", "## Checks", "", "| Check | Result | Detail |", "| --- | --- | --- |"])
    for item in decision["checks"]:
        lines.append(
            f"| {item['name']} | {'pass' if item['passed'] else 'fail'} | {item['detail']} |"
        )
    lines.extend(["", "## Activation Blockers", ""])
    for blocker in decision["activation_blockers"]:
        lines.append(f"- {blocker}")
    lines.extend(
        [
            "",
            "This generated file is release evidence, not consensus activation evidence.",
            "Activation still requires final BIP-360/QRS definitions, Bitcoin Core",
            "validation-path integration, independent reproduction, implementation review,",
            "and reviewer agreement on the batch-Schnorr baseline question.",
            "",
        ]
    )
    return "\n".join(lines)


def evaluate(report: dict[str, Any], batch_evidence: dict[str, Any], allow_unavailable_qrs: bool) -> dict[str, Any]:
    checks: list[dict[str, Any]] = []
    blockers: list[str] = []

    slh_status = status(report, "benchmarks.slh_dsa_sha2_128s")
    qrs_path_status = status(report, "benchmarks.qrs_validation_path")
    schnorr_status = status(report, "benchmarks.schnorr_bip340.individual_valid_verify")
    reviewed_batch_status = status(report, "benchmarks.schnorr_bip340.batch_reviewed_public_api")
    experimental_batch_status = status(report, "benchmarks.schnorr_bip340.batch_experimental_valid")

    add_check(checks, "SLH-DSA timings available", slh_status == "available", slh_status)
    add_check(checks, "QRS validation-path model available", qrs_path_status == "available", qrs_path_status)
    add_check(checks, "Individual Schnorr baseline available", schnorr_status == "available", schnorr_status)
    add_check(
        checks,
        "Reviewed public batch status explicit",
        reviewed_batch_status in {"available", "unavailable"},
        reviewed_batch_status,
    )
    add_check(
        checks,
        "Batch evidence forbids fake speedups",
        "speedup" in batch_evidence.get("policy", "").lower(),
        batch_evidence.get("policy", ""),
    )

    require(schnorr_status == "available", "individual Schnorr baseline is mandatory")
    if not allow_unavailable_qrs:
        require(slh_status == "available", "SLH-DSA timings are mandatory")
        require(qrs_path_status == "available", "QRS validation-path model timings are mandatory")

    if slh_status != "available" or qrs_path_status != "available":
        blockers.append("Native SLH-DSA/QRS timings unavailable on this platform.")
        return {
            "schema": "qrs-native-bench/resource-accounting-decision/v0",
            "draft_rule_status": "evidence_incomplete",
            "activation_ready": False,
            "explicit_qrs_budget_required": False,
            "fallback_budget_status": "not_evaluable",
            "conclusion": "QRS evidence is incomplete on this platform.",
            "p99_ms": {
                "qrs_valid_saturated_block": None,
                "qrs_worst_invalid_fixed_length_saturated_block": None,
                "individual_schnorr_saturated_block": None,
                "experimental_batch_schnorr_saturated_block": None,
                "reviewed_public_batch_schnorr_saturated_block": None,
            },
            "checks": checks,
            "activation_blockers": blockers,
        }

    qrs_valid = p99_ms(report, "block_model.qrs_saturated_block_valid")
    qrs_invalid = p99_ms(report, "block_model.qrs_saturated_block_invalid_fixed_length")
    schnorr_individual = p99_ms(report, "block_model.schnorr_individual_saturated_block")
    experimental_batch = None
    reviewed_batch = None
    if experimental_batch_status == "available":
        experimental_batch = p99_ms(report, "block_model.schnorr_experimental_batch_saturated_block")
    if reviewed_batch_status == "available":
        reviewed_batch = p99_ms(report, "block_model.schnorr_batch_saturated_block")

    qrs_worst = max(qrs_valid, qrs_invalid)
    individual_pass = qrs_worst <= schnorr_individual
    add_check(
        checks,
        "QRS worst p99 below individual Schnorr p99",
        individual_pass,
        f"qrs_worst={qrs_worst:.3f} ms, individual_schnorr={schnorr_individual:.3f} ms",
    )

    experimental_pass = True
    if experimental_batch is not None:
        experimental_pass = qrs_worst <= experimental_batch
        add_check(
            checks,
            "QRS worst p99 below experimental batch Schnorr p99",
            experimental_pass,
            f"qrs_worst={qrs_worst:.3f} ms, experimental_batch={experimental_batch:.3f} ms",
        )
    else:
        blockers.append("Experimental batch Schnorr sensitivity baseline unavailable.")

    if reviewed_batch is None:
        blockers.append("Reviewed public batch Schnorr baseline unavailable.")
    else:
        reviewed_pass = qrs_worst <= reviewed_batch
        add_check(
            checks,
            "QRS worst p99 below reviewed public batch Schnorr p99",
            reviewed_pass,
            f"qrs_worst={qrs_worst:.3f} ms, reviewed_batch={reviewed_batch:.3f} ms",
        )

    qrs_scope = str(nested(report, "benchmarks.qrs_validation_path").get("scope", ""))
    if "not Bitcoin Core consensus integration" in qrs_scope:
        blockers.append("Bitcoin Core validation-path integration not implemented.")
    blockers.append("Final BIP-360/QRS transaction digest definitions not available.")
    blockers.append("Independent cross-hardware reproduction not attached.")
    blockers.append("Second reviewed SLH-DSA implementation not wired.")

    explicit_budget_required = not individual_pass
    if explicit_budget_required:
        draft_status = "fail_explicit_qrs_budget_required"
        fallback_status = "required_before_activation"
        conclusion = (
            "QRS worst-case p99 exceeds individual Schnorr; the draft must add an "
            "explicit per-QRS validation budget before activation."
        )
    elif not experimental_pass:
        draft_status = "activation_unresolved_batch_baseline"
        fallback_status = "reviewer_decision_required"
        conclusion = (
            "QRS is below individual Schnorr but exceeds experimental batch Schnorr; "
            "resource accounting remains unresolved for activation."
        )
    else:
        draft_status = "pass_draft_no_additional_budget_supported"
        fallback_status = "not_required_by_current_draft_evidence"
        conclusion = (
            "Current native evidence supports continuing with the no-additional-budget "
            "rule as Draft-stage review material."
        )

    return {
        "schema": "qrs-native-bench/resource-accounting-decision/v0",
        "draft_rule_status": draft_status,
        "activation_ready": False,
        "explicit_qrs_budget_required": explicit_budget_required,
        "fallback_budget_status": fallback_status,
        "conclusion": conclusion,
        "p99_ms": {
            "qrs_valid_saturated_block": qrs_valid,
            "qrs_worst_invalid_fixed_length_saturated_block": qrs_invalid,
            "individual_schnorr_saturated_block": schnorr_individual,
            "experimental_batch_schnorr_saturated_block": experimental_batch,
            "reviewed_public_batch_schnorr_saturated_block": reviewed_batch,
        },
        "checks": checks,
        "activation_blockers": blockers,
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--report-json", type=Path, required=True)
    parser.add_argument("--batch-evidence-json", type=Path, required=True)
    parser.add_argument("--json", type=Path, default=None)
    parser.add_argument("--markdown", type=Path, default=None)
    parser.add_argument("--allow-unavailable-qrs", action="store_true")
    args = parser.parse_args()

    report = json.loads(args.report_json.read_text(encoding="utf-8"))
    batch_evidence = json.loads(args.batch_evidence_json.read_text(encoding="utf-8"))
    decision = evaluate(report, batch_evidence, args.allow_unavailable_qrs)

    write_text(args.json, json.dumps(decision, indent=2, sort_keys=True) + "\n")
    write_text(args.markdown, render_markdown(decision))
    print(decision["conclusion"])

    if decision["explicit_qrs_budget_required"]:
        return 2
    if decision["draft_rule_status"] == "evidence_incomplete" and not args.allow_unavailable_qrs:
        return 2
    if decision["draft_rule_status"] == "activation_unresolved_batch_baseline":
        return 2
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (DecisionError, KeyError, json.JSONDecodeError) as exc:
        print(f"evaluate_resource_accounting.py: {exc}", file=sys.stderr)
        sys.exit(1)
