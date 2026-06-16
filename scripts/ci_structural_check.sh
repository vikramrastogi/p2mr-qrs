#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT/build"
TMP_DIR="$BUILD_DIR/ci-structural"
QUICK_JSON="$TMP_DIR/quick.json"
QUICK_MD="$TMP_DIR/quick.md"
BATCH_JSON="$TMP_DIR/batch-evidence.json"
BATCH_MD="$TMP_DIR/batch-evidence.md"
RESOURCE_JSON="$TMP_DIR/resource-accounting-decision.json"
RESOURCE_MD="$TMP_DIR/resource-accounting-decision.md"

mkdir -p "$TMP_DIR"

python3 "$ROOT/scripts/compute_qrs_digest_model.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/validate_vector_coverage_matrix.py"
python3 "$ROOT/scripts/validate_consensus_gap_manifest.py"
python3 "$ROOT/scripts/validate_test_vectors.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/verify_qrs_fixtures.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/run_qrs_negative_tests.py"

cmake -S "$ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" -j

python3 "$ROOT/scripts/check_qrs_digest_agreement.py" \
  "$ROOT/test_vectors" \
  --binary "$BUILD_DIR/qrs_native_bench"
python3 "$ROOT/scripts/verify_qrs_vectors.py" \
  "$ROOT/test_vectors" \
  --binary "$BUILD_DIR/qrs_native_bench"

python3 "$ROOT/scripts/check_batch_schnorr_baseline.py" \
  --json "$BATCH_JSON" \
  --markdown "$BATCH_MD" \
  --skip-upstream

"$BUILD_DIR/qrs_native_bench" --quick \
  --json "$QUICK_JSON" \
  --markdown "$QUICK_MD"

extra_args=()
if ! jq -e '.benchmarks.slh_dsa_sha2_128s.status == "available"' "$QUICK_JSON" >/dev/null; then
  extra_args+=(--allow-unavailable-qrs)
fi

python3 "$ROOT/scripts/assert_quick_report.py" \
  --json "$QUICK_JSON" \
  --markdown "$QUICK_MD" \
  --batch-evidence-json "$BATCH_JSON" \
  ${extra_args[@]+"${extra_args[@]}"}
python3 "$ROOT/scripts/evaluate_resource_accounting.py" \
  --report-json "$QUICK_JSON" \
  --batch-evidence-json "$BATCH_JSON" \
  --json "$RESOURCE_JSON" \
  --markdown "$RESOURCE_MD" \
  --advisory \
  ${extra_args[@]+"${extra_args[@]}"}

echo "CI structural check passed"
