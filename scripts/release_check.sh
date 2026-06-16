#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIP="$ROOT/docs/bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki"
DOSSIER="$ROOT/docs/REVIEWER_DOSSIER.md"
CHECKLIST="$ROOT/docs/RELEASE_CHECKLIST.md"
QUICK_JSON="$ROOT/out/quick.json"
QUICK_MD="$ROOT/out/quick.md"
SAMPLE_MD="$ROOT/out/sample-native-report.md"
BATCH_EVIDENCE="$ROOT/out/batch-evidence.json"
BATCH_EVIDENCE_MD="$ROOT/out/batch-evidence.md"
RESOURCE_DECISION_JSON="$ROOT/out/resource-accounting-decision.json"
RESOURCE_DECISION_MD="$ROOT/out/resource-accounting-decision.md"
BUILD_DIR="$ROOT/build"
TMP_DIR="$BUILD_DIR/release-check"
TMP_QUICK_JSON="$TMP_DIR/quick.json"
TMP_QUICK_MD="$TMP_DIR/quick.md"
TMP_BATCH_EVIDENCE="$TMP_DIR/batch-evidence.json"
TMP_BATCH_EVIDENCE_MD="$TMP_DIR/batch-evidence.md"
TMP_RESOURCE_DECISION_JSON="$TMP_DIR/resource-accounting-decision.json"
TMP_RESOURCE_DECISION_MD="$TMP_DIR/resource-accounting-decision.md"
BATCH_DISABLED_JSON="$TMP_DIR/schnorr-batch-disabled.json"
BATCH_DISABLED_MD="$TMP_DIR/schnorr-batch-disabled.md"
SECP_COMMIT_FILE="$ROOT/third_party/secp256k1.COMMIT"
CONSOLIDATED_BIP="$ROOT/docs/bip-p2mr-slh-dsa-leaf-""consolidated.mediawiki"

ensure_out_clean() {
  if ! git -C "$ROOT" diff --quiet -- out; then
    echo "release_check.sh: out/ has unstaged changes; release evidence must be committed or discarded" >&2
    exit 1
  fi
  if ! git -C "$ROOT" diff --cached --quiet -- out; then
    echo "release_check.sh: out/ has staged changes; release evidence must be committed or unstaged" >&2
    exit 1
  fi
}

ensure_out_clean

if [ -e "$CONSOLIDATED_BIP" ]; then
  echo "release_check.sh: use docs/bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki as the single canonical BIP draft" >&2
  exit 1
fi
BIP_V090_COUNT="$(find "$ROOT/docs" -maxdepth 1 -name 'bip-*v0.9.0.mediawiki' -print | wc -l | tr -d ' ')"
if [ "$BIP_V090_COUNT" != "1" ]; then
  echo "release_check.sh: expected exactly one non-archive v0.9.0 BIP draft, found $BIP_V090_COUNT" >&2
  exit 1
fi
if [ ! -f "$BIP" ]; then
  echo "release_check.sh: canonical BIP draft missing: $BIP" >&2
  exit 1
fi
STALE_BIP_PATH="$ROOT/docs/bip-p2mr-quantum-""rescue-leaf-v0.9.0.mediawiki"
if [ -f "$STALE_BIP_PATH" ]; then
  echo "release_check.sh: stale duplicate BIP draft exists: $STALE_BIP_PATH" >&2
  exit 1
fi

grep -n "Version: 0.9.0" "$BIP"
grep -n "Requires: 341, 342, 360" "$BIP"
grep -n "Title: P2MR SLH-DSA Leaf" "$BIP"
grep -n "Authors: Vikram K. Rastogi <vikramrastogi@gmail.com>, Mark H. Dodson <markhdodson@gmail.com>" "$BIP"
grep -n "Assigned: ?" "$BIP"
STALE_TITLE_PATTERN="Title: P2MR Quantum-""Rescue"" Leaf"
if grep -nE "Author:|Created:|$STALE_TITLE_PATTERN" "$BIP"; then
  echo "release_check.sh: stale BIP preamble field found" >&2
  exit 1
fi
STALE_UPPER_QR="Quantum-""Rescue"
STALE_LOWER_QR="quantum-""rescue"
STALE_QSAFE="quantum-""safe"
STALE_STRONG="strong un""forgeability"
STALE_SUF="SUF-""CMA"
STALE_BSD="3-clause B""SD"
STALE_PRELIM="Non-consensus preliminary bench""mark"
STALE_RESCUE_DRAFT="P2MR SLH-DSA Rescue Leaf"
STALE_FULL_TITLE="P2MR Quantum-Rescue Leaf"
STALE_QR_SIGNATURE="Quantum-Rescue Signature"
STALE_DRAFT_CIRCULATED="draft circulated as"
STALE_BIP_WORDING_PATTERN="$STALE_UPPER_QR|$STALE_LOWER_QR|$STALE_QSAFE|$STALE_STRONG|$STALE_SUF|$STALE_BSD|$STALE_PRELIM|$STALE_RESCUE_DRAFT|$STALE_FULL_TITLE|$STALE_QR_SIGNATURE|$STALE_DRAFT_CIRCULATED"
if grep -nE "$STALE_BIP_WORDING_PATTERN" "$BIP"; then
  echo "release_check.sh: stale BIP wording found" >&2
  exit 1
fi
grep -n "hypothesis to test" "$BIP"
grep -n "BIP-360 is still draft" "$BIP"
grep -n "Fixed-length encoding and witness malleability" "$BIP"
grep -n "RESOURCE_ACCOUNTING_DECISION.md" "$BIP"
grep -n "EXPLICIT_QRS_BUDGET_FALLBACK.md" "$BIP"
grep -n "adversarial invalid fixed-length" "$BIP"
grep -n "Draft-stage pre-review" "$ROOT/00_README.md"
grep -n "Draft-stage pre-review" "$ROOT/README.md"
grep -n "verify_qrs_vectors.py" "$CHECKLIST"
grep -n "evaluate_resource_accounting.py" "$CHECKLIST"
grep -n "OpenSSL 3.5 or newer" "$ROOT/README.md" "$ROOT/docs/REPRODUCIBILITY.md" "$CHECKLIST"
grep -n "EXPLICIT_QRS_BUDGET_FALLBACK.md" "$ROOT/docs/RESOURCE_ACCOUNTING_DECISION.md"
grep -n "Bitcoin Core validation-path integration not implemented" "$ROOT/docs/BITCOIN_CORE_INTEGRATION_REQUIREMENTS.md"
grep -n "qrs_transaction_vector.schema.json" "$ROOT/docs/FINAL_TRANSACTION_VECTOR_SCHEMA.md"
grep -n "docs/REPRODUCIBILITY.md" "$ROOT/.github/ISSUE_TEMPLATE/benchmark-reproduction.yml"
grep -n "BIP360_DEPENDENCY_MATRIX.md" "$ROOT/README.md" "$BIP" "$DOSSIER" "$ROOT/docs/PUBLIC_REVIEW_READINESS.md"
grep -n "future leaf version behavior" "$ROOT/docs/BIP360_DEPENDENCY_MATRIX.md"
grep -n "SigMsg extension behavior" "$ROOT/docs/BIP360_DEPENDENCY_MATRIX.md"
grep -n "REPRODUCTION_MATRIX.md" "$ROOT/README.md" "$ROOT/docs/RELEASE_CHECKLIST.md" "$ROOT/docs/REPRODUCIBILITY.md"
grep -n "Apple Silicon macOS" "$ROOT/docs/REPRODUCTION_MATRIX.md"
grep -n "Linux x86_64" "$ROOT/docs/REPRODUCTION_MATRIX.md"
grep -n "minimum public-pre-review target" "$ROOT/docs/REPRODUCTION_MATRIX.md"
grep -n "pre-activation target" "$ROOT/docs/REPRODUCTION_MATRIX.md"
grep -n "SLH_DSA_VERIFY_COST_ANALYSIS.md" "$ROOT/README.md" "$DOSSIER" "$ROOT/docs/RESOURCE_ACCOUNTING_DECISION.md" "$ROOT/docs/REPRODUCIBILITY.md"
grep -n "empirical timing evidence" "$ROOT/docs/SLH_DSA_VERIFY_COST_ANALYSIS.md"
grep -n "algorithmic worst-case" "$ROOT/docs/SLH_DSA_VERIFY_COST_ANALYSIS.md"
grep -n "random bit flips are not a proof" "$ROOT/docs/SLH_DSA_VERIFY_COST_ANALYSIS.md"
grep -n "PUBLIC_REVIEW_POST_DRAFT.md" "$ROOT/README.md" "$ROOT/docs/PUBLIC_REVIEW_READINESS.md" "$CHECKLIST"
grep -n "not activation-ready" "$ROOT/docs/PUBLIC_REVIEW_POST_DRAFT.md"
grep -n "legacy-output policy" "$ROOT/docs/PUBLIC_REVIEW_POST_DRAFT.md"
grep -n "ext_flag=2" "$ROOT/docs/PUBLIC_REVIEW_POST_DRAFT.md"
grep -n "batch-Schnorr sensitivity" "$ROOT/docs/PUBLIC_REVIEW_POST_DRAFT.md"
grep -n "compute_qrs_digest_model.py" "$ROOT/docs/FINAL_TRANSACTION_VECTOR_SCHEMA.md" "$DOSSIER" "$CHECKLIST"
grep -n "cross-implementation" "$ROOT/docs/FINAL_TRANSACTION_VECTOR_SCHEMA.md" "$DOSSIER" "$CHECKLIST"
grep -n "VECTOR_COVERAGE_MATRIX.md" "$ROOT/README.md" "$DOSSIER" "$CHECKLIST"
grep -n "vector_coverage_matrix.json" "$ROOT/docs/VECTOR_COVERAGE_MATRIX.md" "$ROOT/test_vectors/README.md"
grep -n "Final serialized consensus vectors remain blocked" "$ROOT/docs/VECTOR_COVERAGE_MATRIX.md"
grep -n "second reviewed SLH-DSA backend" "$DOSSIER"
grep -n "2.5x hypothetical reviewed-batch-Schnorr baseline" "$ROOT/docs/EXPLICIT_QRS_BUDGET_FALLBACK.md" "$ROOT/docs/RESOURCE_ACCOUNTING_DECISION.md"
grep -n "reviewed public batch-Schnorr implementation becomes available" "$ROOT/docs/EXPLICIT_QRS_BUDGET_FALLBACK.md" "$ROOT/docs/RESOURCE_ACCOUNTING_DECISION.md"
grep -n "fallback trigger" "$ROOT/docs/EXPLICIT_QRS_BUDGET_FALLBACK.md" "$ROOT/docs/RESOURCE_ACCOUNTING_DECISION.md"
grep -Rni "blocking Draft pre-review" "$ROOT/.github/ISSUE_TEMPLATE"
PRIVATE_SCAFFOLD_A="$ROOT/co""dex-specs"
PRIVATE_SCAFFOLD_B="$ROOT/qrs-pre-review-hardening"
if [ -e "$PRIVATE_SCAFFOLD_A" ] || [ -e "$PRIVATE_SCAFFOLD_B" ]; then
  echo "release_check.sh: private scaffolding directories must not be published" >&2
  exit 1
fi
PROCESS_HITS="$(grep -RniE "Cl[a]ude|Co[d]ex|ChatG[P]T|A[I]-generated|\\bA[I]\\b|\\bL[L]M\\b|assist[a]nt" \
  --exclude-dir=.git --exclude-dir=build --exclude-dir=third_party --exclude-dir=__pycache__ \
  --exclude=.git \
  "$ROOT" || true)"
PUBLIC_PROCESS_EXCEPTION="PUBLIC_REVIEW_READINESS.md:.*No private scaffolding, A""I/process artifacts"
PROCESS_HITS="$(printf "%s\n" "$PROCESS_HITS" | grep -v "$PUBLIC_PROCESS_EXCEPTION" || true)"
if [ -n "$PROCESS_HITS" ]; then
  printf "%s\n" "$PROCESS_HITS"
  echo "release_check.sh: non-technical process artifact wording found" >&2
  exit 1
fi

if grep -Rni "consensus test vectors" \
  "$ROOT/README.md" "$ROOT/00_README.md" "$ROOT/docs" "$ROOT/test_vectors"; then
  echo "release_check.sh: vectors must not be described as consensus test vectors" >&2
  exit 1
fi

ACTIVATION_READY_HITS="$(grep -Rni "activation-ready consensus evidence" \
  "$ROOT/README.md" "$ROOT/00_README.md" "$ROOT/docs" || true)"
ACTIVATION_READY_CLAIMS="$(printf "%s\n" "$ACTIVATION_READY_HITS" | grep -vi "not activation-ready consensus evidence" || true)"
if [ -n "$ACTIVATION_READY_CLAIMS" ]; then
  printf "%s\n" "$ACTIVATION_READY_CLAIMS"
  echo "release_check.sh: unsupported activation-ready consensus evidence claim found" >&2
  exit 1
fi

if grep -RniE "QRS is [c]heaper|\\([c]heaper\\)|empirically supported" \
  "$BIP" "$DOSSIER" "$CHECKLIST" "$ROOT/00_README.md" "$ROOT/README.md"; then
  echo "release_check.sh: unsupported cost language found" >&2
  exit 1
fi

grep -n "No legacy-output policy" "$DOSSIER"
grep -n "Batch Schnorr" "$DOSSIER"
grep -n "Known blockers before advancing beyond Draft" "$DOSSIER"

python3 "$ROOT/scripts/compute_qrs_digest_model.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/validate_vector_coverage_matrix.py"
python3 "$ROOT/scripts/validate_test_vectors.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/verify_qrs_fixtures.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/run_qrs_negative_tests.py"

cmake -S "$ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" -j
python3 "$ROOT/scripts/check_qrs_digest_agreement.py" \
  "$ROOT/test_vectors" \
  --binary "$BUILD_DIR/qrs_native_bench"

mkdir -p "$TMP_DIR"
BATCH_EVIDENCE_ARGS=(
  --json "$TMP_BATCH_EVIDENCE"
  --markdown "$TMP_BATCH_EVIDENCE_MD"
)
if [ "${QRS_RELEASE_NETWORK_BATCH:-0}" = "1" ]; then
  BATCH_EVIDENCE_ARGS+=(--upstream-timeout-seconds 15)
else
  BATCH_EVIDENCE_ARGS+=(--skip-upstream)
fi
python3 "$ROOT/scripts/check_batch_schnorr_baseline.py" \
  "${BATCH_EVIDENCE_ARGS[@]}"
python3 "$ROOT/scripts/verify_qrs_vectors.py" \
  "$ROOT/test_vectors" \
  --binary "$BUILD_DIR/qrs_native_bench" \
  --require-crypto
"$BUILD_DIR/qrs_native_bench" --quick --json "$TMP_QUICK_JSON" --markdown "$TMP_QUICK_MD"
if [ "${QRS_RELEASE_UPDATE_ARTIFACTS:-0}" = "1" ]; then
  cp "$TMP_BATCH_EVIDENCE" "$BATCH_EVIDENCE"
  cp "$TMP_BATCH_EVIDENCE_MD" "$BATCH_EVIDENCE_MD"
  cp "$TMP_QUICK_JSON" "$QUICK_JSON"
  cp "$TMP_QUICK_MD" "$QUICK_MD"
  cp "$TMP_QUICK_MD" "$SAMPLE_MD"
fi

CURRENT_COMMIT="$(git -C "$ROOT" rev-parse HEAD)"
REPORT_COMMIT="$(jq -r '.environment.git_commit' "$TMP_QUICK_JSON")"
REPORT_DIRTY="$(jq -r '.environment.working_tree_dirty' "$TMP_QUICK_JSON")"
if [ "$REPORT_COMMIT" != "$CURRENT_COMMIT" ]; then
  echo "release_check.sh: quick report commit $REPORT_COMMIT does not match HEAD $CURRENT_COMMIT" >&2
  exit 1
fi
if [ "$REPORT_DIRTY" != "false" ]; then
  echo "release_check.sh: quick report was generated from a dirty source tree" >&2
  exit 1
fi

EXPECTED_SECP_COMMIT="$(tr -d '[:space:]' < "$SECP_COMMIT_FILE")"
REPORT_SECP_COMMIT="$(jq -r '.benchmarks.schnorr_bip340.libsecp256k1_commit' "$TMP_QUICK_JSON")"
if [ "$REPORT_SECP_COMMIT" != "$EXPECTED_SECP_COMMIT" ]; then
  echo "release_check.sh: reported secp256k1 commit $REPORT_SECP_COMMIT does not match $SECP_COMMIT_FILE $EXPECTED_SECP_COMMIT" >&2
  exit 1
fi
if [ -d "$ROOT/third_party/secp256k1/.git" ]; then
  ACTUAL_SECP_COMMIT="$(git -C "$ROOT/third_party/secp256k1" rev-parse HEAD)"
  if [ "$ACTUAL_SECP_COMMIT" != "$EXPECTED_SECP_COMMIT" ]; then
    echo "release_check.sh: actual secp256k1 commit $ACTUAL_SECP_COMMIT does not match $SECP_COMMIT_FILE $EXPECTED_SECP_COMMIT" >&2
    exit 1
  fi
fi

python3 "$ROOT/scripts/assert_quick_report.py" \
  --json "$TMP_QUICK_JSON" \
  --markdown "$TMP_QUICK_MD" \
  --batch-evidence-json "$TMP_BATCH_EVIDENCE"
QRS_BENCH_DISABLE_EXPERIMENTAL_BATCH=1 \
  "$BUILD_DIR/qrs_native_bench" --quick --only schnorr \
  --json "$BATCH_DISABLED_JSON" \
  --markdown "$BATCH_DISABLED_MD"
jq -e '.benchmarks.schnorr_bip340.individual_valid_verify.status == "available"' \
  "$BATCH_DISABLED_JSON"
jq -e '.benchmarks.schnorr_bip340.individual_invalid_verify.status == "available"' \
  "$BATCH_DISABLED_JSON"
jq -e '.benchmarks.schnorr_bip340.batch_experimental.status == "unavailable"' \
  "$BATCH_DISABLED_JSON"
jq -e '.block_model.schnorr_individual_saturated_block.status == "available"' \
  "$BATCH_DISABLED_JSON"
python3 "$ROOT/scripts/evaluate_resource_accounting.py" \
  --report-json "$TMP_QUICK_JSON" \
  --batch-evidence-json "$TMP_BATCH_EVIDENCE" \
  --json "$TMP_RESOURCE_DECISION_JSON" \
  --markdown "$TMP_RESOURCE_DECISION_MD" \
  --advisory

jq '.benchmarks.slh_dsa_sha2_128s.valid_verify.status' "$TMP_QUICK_JSON"
jq '.benchmarks.slh_dsa_sha2_128s.backends.second_reviewed_backend.status' "$TMP_QUICK_JSON"
jq '.benchmarks.schnorr_bip340.individual_valid_verify.status' "$TMP_QUICK_JSON"
jq '.benchmarks.schnorr_bip340.batch_reviewed_public_api.status' "$TMP_QUICK_JSON"
jq '.benchmarks.schnorr_bip340.batch_experimental.status' "$TMP_QUICK_JSON"
jq '.benchmarks.schnorr_bip340.batch_experimental_challenge_self_test.status' "$TMP_QUICK_JSON"
jq '.benchmarks.qrs_validation_path.total_valid.status' "$TMP_QUICK_JSON"
jq '.environment.git_commit' "$TMP_QUICK_JSON"
jq '.environment.working_tree_dirty' "$TMP_QUICK_JSON"
jq '.environment.benchmark_binary_build_mode' "$TMP_QUICK_JSON"
jq '.environment.benchmark_schema' "$TMP_QUICK_JSON"
jq '.environment.compiler' "$TMP_QUICK_JSON"
jq '.environment.compiler_version' "$TMP_QUICK_JSON"
jq '.environment.compiler_flags' "$TMP_QUICK_JSON"
jq '.environment.cmake_build_type' "$TMP_QUICK_JSON"
jq '.environment.openssl_version' "$TMP_QUICK_JSON"
jq '.environment.openssl_provider' "$TMP_QUICK_JSON"
jq '.draft_rule_status' "$TMP_RESOURCE_DECISION_JSON"
jq '.activation_ready' "$TMP_RESOURCE_DECISION_JSON"
jq '.explicit_qrs_budget_required' "$TMP_RESOURCE_DECISION_JSON"
jq '.batch_sensitivity.status' "$TMP_RESOURCE_DECISION_JSON"
grep -n "Pass/Fail Conclusion" "$TMP_RESOURCE_DECISION_MD"
grep -n "Batch-Speedup Sensitivity" "$TMP_RESOURCE_DECISION_MD"
grep -n "Fallback Trigger Checks" "$TMP_RESOURCE_DECISION_MD"
grep -n "fallback trigger" "$TMP_RESOURCE_DECISION_MD"
grep -n "Hypothetical batch speedups are sensitivity analysis only" "$TMP_RESOURCE_DECISION_MD"
grep -n "does not establish activation readiness" "$TMP_RESOURCE_DECISION_MD"
grep -n "median of per-batch means" "$TMP_QUICK_MD"
grep -n "second reviewed SLH-DSA backend" "$TMP_QUICK_MD"
grep -n "BIP-340 challenge self-test" "$TMP_QUICK_MD" "$DOSSIER"
ensure_out_clean

echo "release checklist passed"
