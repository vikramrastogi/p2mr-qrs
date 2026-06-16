#!/usr/bin/env python3
"""Turn benchmark reports into an explicit resource-accounting decision."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any


HYPOTHETICAL_BATCH_SPEEDUPS = (1.5, 2.0, 2.5, 3.0, 4.0, 5.0)


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


def build_batch_sensitivity(
    qrs_valid: float | None,
    qrs_invalid: float | None,
    schnorr_individual: float | None,
    experimental_batch: float | None,
) -> dict[str, Any]:
    if qrs_valid is None or qrs_invalid is None or schnorr_individual is None:
        return {
            "status": "unavailable",
            "policy": (
                "Hypothetical batch speedups are sensitivity analysis only; "
                "no fake batch baseline is synthesized."
            ),
            "reason": "required QRS or individual Schnorr p99 estimates are unavailable",
            "hypothetical_speedups": [],
        }

    qrs_worst = max(qrs_valid, qrs_invalid)
    experimental_speedup = None
    experimental_tightens = None
    if experimental_batch is not None and experimental_batch > 0:
        experimental_speedup = schnorr_individual / experimental_batch
        experimental_tightens = experimental_batch < schnorr_individual

    hypothetical: list[dict[str, Any]] = []
    for speedup in HYPOTHETICAL_BATCH_SPEEDUPS:
        synthetic_schnorr = schnorr_individual / speedup
        hypothetical.append(
            {
                "speedup_vs_individual": speedup,
                "schnorr_p99_ms": synthetic_schnorr,
                "qrs_valid_exceeds": qrs_valid > synthetic_schnorr,
                "qrs_worst_invalid_exceeds": qrs_worst > synthetic_schnorr,
            }
        )

    return {
        "status": "available",
        "policy": (
            "Hypothetical batch speedups are sensitivity analysis only; "
            "they are not a measured baseline and no fake batch baseline is synthesized."
        ),
        "experimental_batch_tightens_individual": experimental_tightens,
        "experimental_batch_speedup_vs_individual": experimental_speedup,
        "qrs_valid_exceeds_if_batch_speedup_gt": schnorr_individual / qrs_valid,
        "qrs_worst_invalid_exceeds_if_batch_speedup_gt": schnorr_individual / qrs_worst,
        "hypothetical_speedups": hypothetical,
    }


def build_fallback_triggers(
    qrs_valid: float | None,
    qrs_invalid: float | None,
    schnorr_individual: float | None,
    reviewed_batch: float | None,
) -> dict[str, Any]:
    triggers: list[dict[str, Any]] = []
    if qrs_valid is None or qrs_invalid is None or schnorr_individual is None:
        return {
            "status": "unavailable",
            "policy": "fallback trigger evaluation requires QRS and individual Schnorr p99 estimates.",
            "triggers": triggers,
        }

    qrs_worst = max(qrs_valid, qrs_invalid)

    def add(name: str, threshold: float | None, detail: str) -> None:
        triggers.append(
            {
                "name": name,
                "triggered": None if threshold is None else qrs_worst > threshold,
                "threshold_p99_ms": threshold,
                "qrs_worst_p99_ms": qrs_worst,
                "detail": detail,
            }
        )

    add(
        "qrs_worst_exceeds_individual_schnorr",
        schnorr_individual,
        "fallback trigger if QRS worst-invalid p99 exceeds individual Schnorr p99",
    )
    add(
        "qrs_worst_exceeds_2_5x_hypothetical_reviewed_batch_schnorr",
        schnorr_individual / 2.5,
        "fallback trigger if QRS worst-invalid p99 exceeds a 2.5x hypothetical reviewed-batch-Schnorr baseline",
    )
    add(
        "qrs_worst_exceeds_3_0x_hypothetical_reviewed_batch_schnorr",
        schnorr_individual / 3.0,
        "fallback trigger if QRS worst-invalid p99 exceeds a 3.0x hypothetical reviewed-batch-Schnorr baseline",
    )
    add(
        "qrs_worst_exceeds_reviewed_public_batch_schnorr",
        reviewed_batch,
        "fallback trigger if a reviewed public batch-Schnorr implementation becomes available and QRS exceeds it",
    )
    triggers.append(
        {
            "name": "bitcoin_core_validation_path_overhead",
            "triggered": None,
            "threshold_p99_ms": None,
            "qrs_worst_p99_ms": qrs_worst,
            "detail": "fallback trigger requires Bitcoin Core validation-path integration evidence; current harness models that path only",
        }
    )

    return {
        "status": "available",
        "policy": (
            "Fallback trigger checks identify when the inactive explicit QRS budget "
            "must be reconsidered before activation."
        ),
        "triggers": triggers,
    }


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
    sensitivity = decision.get("batch_sensitivity", {})
    lines.extend(
        [
            "",
            "## Batch-Speedup Sensitivity",
            "",
            sensitivity.get(
                "policy",
                "Hypothetical batch speedups are sensitivity analysis only; no fake batch baseline is synthesized.",
            ),
            "",
            "If the experimental batch baseline is slower than individual Schnorr, "
            "it is reported as non-tightening and cannot support the "
            "no-additional-budget rule for activation review.",
            "",
        ]
    )
    if sensitivity.get("status") == "available":
        experimental_tightens = sensitivity.get("experimental_batch_tightens_individual")
        experimental_speedup = sensitivity.get("experimental_batch_speedup_vs_individual")
        lines.append(
            "- Experimental batch tightens individual Schnorr: "
            + ("unavailable" if experimental_tightens is None else str(experimental_tightens).lower())
        )
        if experimental_speedup is not None:
            lines.append(f"- Experimental batch speedup vs individual: {experimental_speedup:.3f}x")
        lines.append(
            "- QRS valid exceeds a hypothetical batch baseline if speedup is greater than "
            f"{sensitivity['qrs_valid_exceeds_if_batch_speedup_gt']:.3f}x."
        )
        lines.append(
            "- QRS worst-invalid exceeds a hypothetical batch baseline if speedup is greater than "
            f"{sensitivity['qrs_worst_invalid_exceeds_if_batch_speedup_gt']:.3f}x."
        )
        lines.extend(
            [
                "",
                "| Hypothetical speedup vs individual | Hypothetical Schnorr p99 ms | QRS valid exceeds | QRS worst-invalid exceeds |",
                "| ---: | ---: | --- | --- |",
            ]
        )
        for item in sensitivity["hypothetical_speedups"]:
            lines.append(
                f"| {item['speedup_vs_individual']:.1f}x | "
                f"{item['schnorr_p99_ms']:.3f} | "
                f"{str(item['qrs_valid_exceeds']).lower()} | "
                f"{str(item['qrs_worst_invalid_exceeds']).lower()} |"
            )
    else:
        lines.append(f"- unavailable: {sensitivity.get('reason', 'unknown')}")
    fallback = decision.get("fallback_triggers", {})
    lines.extend(
        [
            "",
            "## Fallback Trigger Checks",
            "",
            fallback.get(
                "policy",
                "Fallback trigger evaluation is unavailable for this report.",
            ),
            "",
            "| Fallback trigger | Triggered | Threshold p99 ms | QRS worst p99 ms | Detail |",
            "| --- | --- | ---: | ---: | --- |",
        ]
    )
    for item in fallback.get("triggers", []):
        threshold = item.get("threshold_p99_ms")
        qrs_worst = item.get("qrs_worst_p99_ms")
        triggered = item.get("triggered")
        lines.append(
            f"| {item['name']} | "
            f"{'unavailable' if triggered is None else str(triggered).lower()} | "
            f"{'unavailable' if threshold is None else f'{threshold:.3f}'} | "
            f"{'unavailable' if qrs_worst is None else f'{qrs_worst:.3f}'} | "
            f"{item['detail']} |"
        )
    lines.extend(["", "## Activation Blockers", ""])
    for blocker in decision["activation_blockers"]:
        lines.append(f"- {blocker}")
    lines.extend(
        [
            "",
            "This generated file does not establish activation readiness.",
            "It is release evidence, not consensus activation evidence.",
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
    required_metadata = [
        "git_commit",
        "working_tree_dirty",
        "benchmark_binary_build_mode",
        "compiler",
        "compiler_version",
        "compiler_flags",
        "cmake_build_type",
        "openssl_version",
        "openssl_provider",
        "benchmark_schema",
    ]
    env = report.get("environment", {})
    missing_metadata = [key for key in required_metadata if not env.get(key)]

    slh_status = status(report, "benchmarks.slh_dsa_sha2_128s")
    qrs_path_status = status(report, "benchmarks.qrs_validation_path")
    schnorr_status = status(report, "benchmarks.schnorr_bip340.individual_valid_verify")
    reviewed_batch_status = status(report, "benchmarks.schnorr_bip340.batch_reviewed_public_api")
    experimental_batch_status = status(report, "benchmarks.schnorr_bip340.batch_experimental_valid")
    invalid_status = status(report, "benchmarks.slh_dsa_sha2_128s.invalid_fixed_length_verify")

    add_check(checks, "SLH-DSA timings available", slh_status == "available", slh_status)
    add_check(
        checks,
        "Invalid fixed-length SLH-DSA timings available",
        invalid_status == "available",
        invalid_status,
    )
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
    add_check(
        checks,
        "Report metadata complete",
        not missing_metadata,
        "complete" if not missing_metadata else ", ".join(missing_metadata),
    )

    require(schnorr_status == "available", "individual Schnorr baseline is mandatory")

    if (
        slh_status != "available"
        or invalid_status != "available"
        or qrs_path_status != "available"
        or missing_metadata
    ):
        if slh_status != "available" or qrs_path_status != "available":
            blockers.append("Native SLH-DSA/QRS timings unavailable on this platform.")
        if invalid_status != "available":
            blockers.append("Invalid fixed-length QRS timing unavailable.")
        if missing_metadata:
            blockers.append("Report metadata incomplete: " + ", ".join(missing_metadata) + ".")
        if allow_unavailable_qrs and (slh_status != "available" or qrs_path_status != "available"):
            draft_status = "not_evaluable_on_this_platform"
            fallback_status = "not_evaluable_on_this_platform"
            conclusion = "QRS evidence is incomplete on this platform."
            explicit_required = False
        else:
            draft_status = "explicit_budget_required_before_activation"
            fallback_status = "required_before_activation"
            conclusion = (
                "Native QRS evidence or required report metadata is missing; the draft "
                "cannot advance without an explicit per-QRS validation budget or new evidence."
            )
        return {
            "schema": "qrs-native-bench/resource-accounting-decision/v0",
            "draft_rule_status": draft_status,
            "activation_ready": False,
            "explicit_qrs_budget_required": explicit_required,
            "fallback_budget_status": fallback_status,
            "conclusion": conclusion,
            "p99_ms": {
                "qrs_valid_saturated_block": None,
                "qrs_worst_invalid_fixed_length_saturated_block": None,
                "individual_schnorr_saturated_block": None,
                "experimental_batch_schnorr_saturated_block": None,
                "reviewed_public_batch_schnorr_saturated_block": None,
            },
            "batch_sensitivity": build_batch_sensitivity(None, None, None, None),
            "fallback_triggers": build_fallback_triggers(None, None, None, None),
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
        experimental_tightens_individual = experimental_batch < schnorr_individual
        add_check(
            checks,
            "Experimental batch tightens individual Schnorr baseline",
            experimental_tightens_individual,
            f"experimental_batch={experimental_batch:.3f} ms, individual_schnorr={schnorr_individual:.3f} ms",
        )
    else:
        experimental_pass = False
        experimental_tightens_individual = False
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
        draft_status = "explicit_budget_required_before_activation"
        fallback_status = "required_before_activation"
        conclusion = (
            "QRS worst-case p99 exceeds individual Schnorr; the draft must add an "
            "explicit per-QRS validation budget before activation."
        )
    elif not experimental_pass or not experimental_tightens_individual:
        draft_status = "unresolved_requires_reviewer_decision"
        fallback_status = "reviewer_decision_required"
        if experimental_batch is not None and not experimental_tightens_individual:
            conclusion = (
                "QRS is below individual Schnorr, but the experimental batch Schnorr "
                "baseline does not tighten the individual baseline; resource accounting "
                "remains unresolved for activation."
            )
        else:
            conclusion = (
                "QRS is below individual Schnorr but exceeds experimental batch Schnorr; "
                "resource accounting remains unresolved for activation."
            )
    else:
        draft_status = "supports_no_additional_budget_for_draft_review"
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
        "batch_sensitivity": build_batch_sensitivity(
            qrs_valid, qrs_invalid, schnorr_individual, experimental_batch
        ),
        "fallback_triggers": build_fallback_triggers(
            qrs_valid, qrs_invalid, schnorr_individual, reviewed_batch
        ),
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
    parser.add_argument(
        "--advisory",
        action="store_true",
        help="Emit the decision but do not fail on timing-derived budget decisions. Use only for quick smoke reports.",
    )
    args = parser.parse_args()

    report = json.loads(args.report_json.read_text(encoding="utf-8"))
    batch_evidence = json.loads(args.batch_evidence_json.read_text(encoding="utf-8"))
    decision = evaluate(report, batch_evidence, args.allow_unavailable_qrs)

    write_text(args.json, json.dumps(decision, indent=2, sort_keys=True) + "\n")
    write_text(args.markdown, render_markdown(decision))
    print(decision["conclusion"])

    if decision["explicit_qrs_budget_required"] and not args.advisory:
        return 2
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (DecisionError, KeyError, json.JSONDecodeError) as exc:
        print(f"evaluate_resource_accounting.py: {exc}", file=sys.stderr)
        sys.exit(1)
