#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT/build"
TMP_DIR="$BUILD_DIR/full-release-check"
STANDARD_JSON="$ROOT/out/standard.json"
STANDARD_MD="$ROOT/out/standard.md"
BATCH_EVIDENCE="$ROOT/out/batch-evidence.json"
BATCH_EVIDENCE_MD="$ROOT/out/batch-evidence.md"
RESOURCE_DECISION_JSON="$ROOT/out/resource-accounting-decision.json"
RESOURCE_DECISION_MD="$ROOT/out/resource-accounting-decision.md"
TMP_STANDARD_JSON="$TMP_DIR/standard.json"
TMP_STANDARD_MD="$TMP_DIR/standard.md"
TMP_BATCH_EVIDENCE="$TMP_DIR/batch-evidence.json"
TMP_BATCH_EVIDENCE_MD="$TMP_DIR/batch-evidence.md"
TMP_RESOURCE_DECISION_JSON="$TMP_DIR/resource-accounting-decision.json"
TMP_RESOURCE_DECISION_MD="$TMP_DIR/resource-accounting-decision.md"
SECP_COMMIT_FILE="$ROOT/third_party/secp256k1.COMMIT"

ensure_out_clean() {
  if ! git -C "$ROOT" diff --quiet -- out; then
    echo "full_release_check.sh: out/ has unstaged changes; release evidence must be committed or discarded" >&2
    exit 1
  fi
  if ! git -C "$ROOT" diff --cached --quiet -- out; then
    echo "full_release_check.sh: out/ has staged changes; release evidence must be committed or unstaged" >&2
    exit 1
  fi
}

ensure_out_clean

cmake -S "$ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" -j

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

python3 "$ROOT/scripts/validate_test_vectors.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/compute_qrs_digest_model.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/validate_vector_coverage_matrix.py"
python3 "$ROOT/scripts/verify_qrs_fixtures.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/run_qrs_negative_tests.py"
python3 "$ROOT/scripts/verify_qrs_vectors.py" \
  "$ROOT/test_vectors" \
  --binary "$BUILD_DIR/qrs_native_bench" \
  --require-crypto
python3 "$ROOT/scripts/check_qrs_digest_agreement.py" \
  "$ROOT/test_vectors" \
  --binary "$BUILD_DIR/qrs_native_bench"

"$BUILD_DIR/qrs_native_bench" \
  --standard \
  --json "$TMP_STANDARD_JSON" \
  --markdown "$TMP_STANDARD_MD"

CURRENT_COMMIT="$(git -C "$ROOT" rev-parse HEAD)"
REPORT_COMMIT="$(jq -r '.environment.git_commit' "$TMP_STANDARD_JSON")"
REPORT_DIRTY="$(jq -r '.environment.working_tree_dirty' "$TMP_STANDARD_JSON")"
if [ "$REPORT_COMMIT" != "$CURRENT_COMMIT" ]; then
  echo "full_release_check.sh: standard report commit $REPORT_COMMIT does not match HEAD $CURRENT_COMMIT" >&2
  exit 1
fi
if [ "$REPORT_DIRTY" != "false" ]; then
  echo "full_release_check.sh: standard report was generated from a dirty source tree" >&2
  exit 1
fi

EXPECTED_SECP_COMMIT="$(tr -d '[:space:]' < "$SECP_COMMIT_FILE")"
REPORT_SECP_COMMIT="$(jq -r '.benchmarks.schnorr_bip340.libsecp256k1_commit' "$TMP_STANDARD_JSON")"
if [ "$REPORT_SECP_COMMIT" != "$EXPECTED_SECP_COMMIT" ]; then
  echo "full_release_check.sh: reported secp256k1 commit $REPORT_SECP_COMMIT does not match $SECP_COMMIT_FILE $EXPECTED_SECP_COMMIT" >&2
  exit 1
fi
if [ -d "$ROOT/third_party/secp256k1/.git" ]; then
  ACTUAL_SECP_COMMIT="$(git -C "$ROOT/third_party/secp256k1" rev-parse HEAD)"
  if [ "$ACTUAL_SECP_COMMIT" != "$EXPECTED_SECP_COMMIT" ]; then
    echo "full_release_check.sh: actual secp256k1 commit $ACTUAL_SECP_COMMIT does not match $SECP_COMMIT_FILE $EXPECTED_SECP_COMMIT" >&2
    exit 1
  fi
fi

python3 "$ROOT/scripts/assert_quick_report.py" \
  --json "$TMP_STANDARD_JSON" \
  --markdown "$TMP_STANDARD_MD" \
  --batch-evidence-json "$TMP_BATCH_EVIDENCE"

python3 "$ROOT/scripts/evaluate_resource_accounting.py" \
  --report-json "$TMP_STANDARD_JSON" \
  --batch-evidence-json "$TMP_BATCH_EVIDENCE" \
  --json "$TMP_RESOURCE_DECISION_JSON" \
  --markdown "$TMP_RESOURCE_DECISION_MD"

ACTIVATION_READY="$(jq -r '.activation_ready' "$TMP_RESOURCE_DECISION_JSON")"
if [ "$ACTIVATION_READY" != "false" ]; then
  echo "full_release_check.sh: expected activation_ready=false, got $ACTIVATION_READY" >&2
  exit 1
fi
EXPLICIT_QRS_BUDGET_REQUIRED="$(jq -r '.explicit_qrs_budget_required' "$TMP_RESOURCE_DECISION_JSON")"
if [ "$EXPLICIT_QRS_BUDGET_REQUIRED" != "false" ]; then
  echo "full_release_check.sh: current evidence requires an explicit QRS validation budget" >&2
  exit 1
fi
DRAFT_RULE_STATUS="$(jq -r '.draft_rule_status' "$TMP_RESOURCE_DECISION_JSON")"
if [ "$DRAFT_RULE_STATUS" != "supports_no_additional_budget_for_draft_review" ]; then
  echo "full_release_check.sh: current resource-accounting decision is $DRAFT_RULE_STATUS, not supports_no_additional_budget_for_draft_review" >&2
  exit 1
fi

if [ "${QRS_RELEASE_UPDATE_ARTIFACTS:-0}" = "1" ]; then
  cp "$TMP_STANDARD_JSON" "$STANDARD_JSON"
  cp "$TMP_STANDARD_MD" "$STANDARD_MD"
  cp "$TMP_BATCH_EVIDENCE" "$BATCH_EVIDENCE"
  cp "$TMP_BATCH_EVIDENCE_MD" "$BATCH_EVIDENCE_MD"
  cp "$TMP_RESOURCE_DECISION_JSON" "$RESOURCE_DECISION_JSON"
  cp "$TMP_RESOURCE_DECISION_MD" "$RESOURCE_DECISION_MD"
fi

ensure_out_clean

echo "full release checklist passed"
