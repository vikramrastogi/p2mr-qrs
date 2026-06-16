#!/usr/bin/env python3
"""Assert the reviewer-facing quick-report contract."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path


def fail(message: str) -> None:
    raise AssertionError(message)


def require(value: bool, message: str) -> None:
    if not value:
        fail(message)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("positional", nargs="*", type=Path)
    parser.add_argument("--json", type=Path, default=None)
    parser.add_argument("--markdown", type=Path, default=None)
    parser.add_argument("--batch-evidence-json", type=Path, default=None)
    parser.add_argument(
        "--allow-unavailable-qrs",
        action="store_true",
        help="Allow QRS/SLH-DSA timings to be unavailable on platforms whose OpenSSL lacks SLH-DSA.",
    )
    args = parser.parse_args()
    if args.json is None:
        if not args.positional:
            parser.error("either --json or positional JSON path is required")
        args.json = args.positional[0]
    if args.markdown is None and len(args.positional) >= 2:
        args.markdown = args.positional[1]
    if len(args.positional) > 2:
        parser.error("too many positional arguments")

    report = json.loads(args.json.read_text(encoding="utf-8"))
    require(report.get("schema") == "qrs-native-bench/v0", "unexpected report schema")
    env = report["environment"]
    require(env.get("git_commit"), "environment.git_commit must be present")
    require(
        env.get("working_tree_dirty") in {"true", "false", "unavailable"},
        "environment.working_tree_dirty must be true, false, or unavailable",
    )
    require(
        env.get("benchmark_binary_build_mode"),
        "environment.benchmark_binary_build_mode must be present",
    )
    require(env.get("benchmark_schema") == "qrs-native-bench/v0", "environment.benchmark_schema mismatch")
    require(env.get("cmake_build_type"), "environment.cmake_build_type must be present")
    require(env.get("compiler"), "environment.compiler must be present")
    require(env.get("compiler_version"), "environment.compiler_version must be present")
    require(env.get("compiler_flags"), "environment.compiler_flags must be present")
    require(env.get("openssl_version"), "environment.openssl_version must be present")
    require(env.get("openssl_provider"), "environment.openssl_provider must be present")

    schnorr = report["benchmarks"]["schnorr_bip340"]
    require(schnorr["status"] == "available", "individual Schnorr backend must be available")
    require(
        schnorr["individual_valid_verify"]["status"] == "available",
        "individual valid Schnorr timing must be available",
    )
    require(
        schnorr["individual_invalid_verify"]["status"] == "available",
        "individual invalid Schnorr timing must be available",
    )

    batch = schnorr["batch"]
    require(batch["status"] == "unavailable", "batch status must remain explicitly unavailable")
    require(
        schnorr["batch_reviewed_public_api_status"] == "unavailable",
        "reviewed public batch API status must remain unavailable",
    )
    require(
        schnorr["batch_reviewed_public_api"]["status"] == "unavailable",
        "reviewed public batch API object status must remain unavailable",
    )
    require(
        batch["per_signature_verify"]["status"] == "unavailable",
        "batch per-signature timing must not be synthesized",
    )
    require("verify_batch" in batch["reason"], "batch reason must mention verify_batch limitation")
    require("No fake batch baseline" in batch["reason"], "batch reason must forbid fake baseline")
    experimental_available = schnorr["batch_experimental"]["status"] == "available"
    if experimental_available:
        require(
            schnorr["batch_experimental_valid"]["status"] == "available",
            "experimental valid batch baseline must be available when experimental status is available",
        )
        require(
            schnorr["batch_experimental_invalid"]["status"] == "available",
            "experimental invalid batch baseline must be available when experimental status is available",
        )
        require(
            schnorr["batch_experimental_challenge_self_test"]["status"] == "available",
            "experimental batch must report a passing BIP-340 challenge self-test",
        )
        require(
            schnorr["batch_experimental_challenge_self_test"]["result"] == "passed",
            "experimental batch challenge self-test result must be passed",
        )
        require(
            "independent BIP0340/challenge tagged-hash" in schnorr["batch_experimental_challenge_self_test"]["description"],
            "BIP-340 challenge self-test description must identify independent tagged-hash comparison",
        )
        require(
            "tagged_hash" in schnorr["batch_experimental_challenge_self_test"]["reason"],
            "BIP-340 challenge self-test reason must identify independent tagged_hash construction",
        )
    else:
        require(
            schnorr["batch_experimental"]["status"] == "unavailable",
            "experimental batch status must be available or explicitly unavailable",
        )
        require(
            schnorr["batch_experimental"]["reason"],
            "unavailable experimental batch status must report a reason",
        )
    require(
        "not a reviewed public libsecp256k1 API" in schnorr["batch_experimental_valid"]["label"],
        "experimental batch label must say it is not a reviewed public API",
    )
    require(
        "coefficient derivation" in schnorr["batch_experimental_valid"]["timing_scope"],
        "experimental batch timing scope must document coefficient derivation",
    )
    if experimental_available:
        require(
            len(schnorr["batch_experimental_valid"]["batch_sizes"]) >= 5,
            "experimental batch baseline must report all requested batch sizes",
        )

    block = report["block_model"]
    require(
        block["schnorr_individual_saturated_block"]["status"] == "available",
        "individual Schnorr block model must be available",
    )
    require(
        block["schnorr_batch_saturated_block"]["status"] == "unavailable",
        "batch Schnorr block model must remain unavailable",
    )
    require(
        block["schnorr_experimental_batch_saturated_block"]["status"]
        == ("available" if experimental_available else "unavailable"),
        "experimental batch block model must match experimental batch availability",
    )

    slh = report["benchmarks"]["slh_dsa_sha2_128s"]
    slh_backends = slh.get("backends", {})
    require("openssl" in slh_backends, "SLH-DSA backend inventory must include OpenSSL")
    require(
        "second_reviewed_backend" in slh_backends,
        "SLH-DSA backend inventory must include second reviewed backend status",
    )
    require(
        slh_backends["second_reviewed_backend"]["status"] == "unavailable",
        "second reviewed SLH-DSA backend must remain explicitly unavailable",
    )
    require(
        slh_backends["second_reviewed_backend"].get("reason"),
        "second reviewed SLH-DSA backend must report a reason",
    )
    if slh["status"] == "available":
        require(slh["public_key_bytes"] == 32, "SLH-DSA public key must be 32 bytes")
        require(slh["signature_bytes"] == 7856, "SLH-DSA signature must be 7856 bytes")
        require(
            slh_backends["openssl"]["status"] == "available",
            "OpenSSL SLH-DSA backend inventory must match available SLH timings",
        )
        require(slh["valid_verify"]["status"] == "available", "valid SLH timing missing")
        require(
            slh["invalid_fixed_length_verify"]["status"] == "available",
            "invalid fixed-length SLH timing missing",
        )
        case_names = set(slh["invalid_fixed_length_cases"].keys())
        require(
            len(case_names) >= 9,
            "invalid fixed-length SLH timing suite must include adversarial mutations",
        )
        for case_name in ["wrong_message", "wrong_public_key"]:
            require(
                case_name in case_names,
                f"invalid fixed-length SLH timing suite must include {case_name}",
            )
        for key in [
            "invalid_fixed_length_min_observed",
            "invalid_fixed_length_best_observed",
            "invalid_fixed_length_median_observed",
            "invalid_fixed_length_p99_observed",
            "invalid_fixed_length_worst_observed",
        ]:
            require(slh[key]["status"] == "available", f"{key} timing missing")
        require(
            slh["invalid_fixed_length_case_name_worst"] in slh["invalid_fixed_length_cases"],
            "worst invalid fixed-length case name must identify a reported case",
        )
        require(
            slh["invalid_fixed_length_verify"]["p99_ns"]
            == slh["invalid_fixed_length_worst_observed"]["p99_ns"],
            "compatibility invalid_fixed_length_verify must alias worst observed p99 case",
        )
    else:
        require(args.allow_unavailable_qrs, "SLH-DSA backend must be available")
        require(slh.get("reason"), "unavailable SLH-DSA backend must report a reason")
        require(
            "EVP_PKEY_CTX_new_from_name" in slh["reason"] or "unsupported" in slh["reason"],
            "unavailable SLH-DSA reason must identify OpenSSL algorithm support",
        )

    qrs_path = report["benchmarks"]["qrs_validation_path"]
    if qrs_path["status"] == "available":
        require(
            qrs_path["total_valid"]["status"] == "available",
            "QRS path total valid timing missing",
        )
        require(
            qrs_path["invalid_structural"]["status"] == "available",
            "QRS path invalid structural timing missing",
        )
        require(
            qrs_path["invalid_fixed_length"]["status"] == "available",
            "QRS path invalid fixed-length timing missing",
        )
    else:
        require(args.allow_unavailable_qrs, "QRS path timings must be available")
        require(qrs_path.get("reason"), "unavailable QRS path must report a reason")
        require(
            "EVP_PKEY_CTX_new_from_name" in qrs_path["reason"]
            or "unsupported" in qrs_path["reason"],
            "unavailable QRS path reason must identify OpenSSL algorithm support",
        )
    require(
        "not Bitcoin Core consensus integration" in qrs_path["scope"],
        "QRS path scope must avoid claiming Bitcoin Core integration",
    )

    if args.markdown:
        md = args.markdown.read_text(encoding="utf-8")
        require("batch Schnorr" in md, "Markdown must mention batch Schnorr")
        require("reviewed public batch" in md, "Markdown must mention reviewed public batch")
        require("No fake batch baseline" in md, "Markdown must include the no-fake baseline reason")
        require(
            "experimental native BIP-340 batch baseline" in md,
            "Markdown must identify the experimental native BIP-340 batch baseline",
        )
        require(
            "not a reviewed public libsecp256k1 API" in md,
            "Markdown must distinguish experimental batch from reviewed public API",
        )
        require(
            "BIP-340 challenge self-test" in md,
            "Markdown must report the BIP-340 challenge self-test",
        )
        require(
            "QRS vs experimental batch Schnorr" in md,
            "Markdown must include QRS vs experimental batch Schnorr section",
        )
        require(
            "SLH-DSA Invalid Fixed-Length Suite" in md,
            "Markdown must include adversarial invalid fixed-length suite",
        )
        require(
            "Worst observed invalid fixed-length case" in md,
            "Markdown must identify the worst invalid fixed-length case",
        )
        require(
            "median of per-batch means" in md,
            "Markdown must define timing-statistic samples",
        )
        require("QRS Validation Path Model" in md, "Markdown must include QRS path model")

    if args.batch_evidence_json:
        evidence = json.loads(args.batch_evidence_json.read_text(encoding="utf-8"))
        require(evidence["status"] == "unavailable", "batch evidence status must be unavailable")
        require(
            evidence["bundled_commit_expected"] == schnorr["libsecp256k1_commit"],
            "batch evidence commit must match report commit",
        )
        require(
            evidence["public_batch_api_detected"] is False,
            "batch evidence must not detect a public batch API",
        )

    print("quick report assertions passed")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except AssertionError as exc:
        print(f"assert_quick_report.py: {exc}", file=sys.stderr)
        sys.exit(1)
