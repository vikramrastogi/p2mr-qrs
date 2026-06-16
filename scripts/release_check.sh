#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIP="$ROOT/docs/spec/bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki"
QUICK_JSON="$ROOT/out/quick.json"
QUICK_MD="$ROOT/out/quick.md"
SAMPLE_MD="$ROOT/out/sample-native-report.md"
STANDARD_JSON="$ROOT/out/standard.json"
FULL_JSON="$ROOT/out/full.json"
BATCH_EVIDENCE="$ROOT/out/batch-evidence.json"
BATCH_EVIDENCE_MD="$ROOT/out/batch-evidence.md"
RESOURCE_DECISION_JSON="$ROOT/out/resource-accounting-decision.json"
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
CONSENSUS_GAP_MANIFEST="$ROOT/docs/evidence/consensus-gap-manifest.json"
CONSENSUS_GAP_MANIFEST_MD="$ROOT/docs/evidence/consensus-gap-manifest.md"
BATCH_SCHNORR_STATUS="$ROOT/docs/evidence/batch-schnorr-baseline-status.md"
START_HERE="$ROOT/docs/review/start-here.md"
REPRODUCIBILITY="$ROOT/docs/reproducibility/reproducibility.md"

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

validate_committed_sample_reports() {
  local sample_commit
  sample_commit="$(jq -r '.environment.git_commit' "$QUICK_JSON")"
  if [ -z "$sample_commit" ] || [ "$sample_commit" = "null" ]; then
    echo "release_check.sh: committed quick sample report is missing environment.git_commit" >&2
    exit 1
  fi
  if ! git -C "$ROOT" merge-base --is-ancestor "$sample_commit" HEAD; then
    echo "release_check.sh: committed sample report commit $sample_commit is not reachable from HEAD" >&2
    exit 1
  fi
  for report in "$QUICK_JSON" "$STANDARD_JSON" "$FULL_JSON"; do
    jq -e --arg commit "$sample_commit" \
      '.environment.git_commit == $commit and .environment.working_tree_dirty == "false"' \
      "$report" >/dev/null
  done
  grep -n "$sample_commit" "$ROOT/docs/evidence/resource-accounting-decision.md"
  jq -n -e \
    --slurpfile full_report "$FULL_JSON" \
    --slurpfile decision_report "$RESOURCE_DECISION_JSON" '
      ($decision_report[0].p99_ms.qrs_valid_saturated_block
        == $full_report[0].block_model.qrs_saturated_block_valid.p99_ms) and
      ($decision_report[0].p99_ms.qrs_worst_invalid_fixed_length_saturated_block
        == $full_report[0].block_model.qrs_saturated_block_invalid_fixed_length.p99_ms) and
      ($decision_report[0].p99_ms.individual_schnorr_saturated_block
        == $full_report[0].block_model.schnorr_individual_saturated_block.p99_ms) and
      ($decision_report[0].p99_ms.experimental_batch_schnorr_saturated_block
        == $full_report[0].block_model.schnorr_experimental_batch_saturated_block.p99_ms)
    ' >/dev/null || {
      echo "release_check.sh: committed resource-accounting decision does not match out/full.json" >&2
      exit 1
    }
}

ensure_out_clean
validate_committed_sample_reports

BIP_V090_COUNT="$(find "$ROOT/docs/spec" -maxdepth 1 -name 'bip-*v0.9.0.mediawiki' -print | wc -l | tr -d ' ')"
if [ "$BIP_V090_COUNT" != "1" ]; then
  echo "release_check.sh: expected exactly one non-archive v0.9.0 BIP draft, found $BIP_V090_COUNT" >&2
  exit 1
fi
if [ ! -f "$BIP" ]; then
  echo "release_check.sh: canonical BIP draft missing: $BIP" >&2
  exit 1
fi

grep -n "Version: 0.9.0" "$BIP"
grep -n "Requires: 341, 342, 360" "$BIP"
grep -n "Title: P2MR SLH-DSA Leaf" "$BIP"
grep -n "Authors: Vikram Rastogi <vikramrastogi@gmail.com>, Mark Dodson <markhdodson@gmail.com>" "$BIP"
grep -n "Assigned: ?" "$BIP"
if grep -nE "Author:|Created:|Title: P2MR Quantum-Rescue Leaf" "$BIP"; then
  echo "release_check.sh: stale BIP preamble field found" >&2
  exit 1
fi
if grep -nE "Quantum-Rescue|quantum-rescue|quantum-safe|strong unforgeability|SUF-CMA|3-clause BSD|Non-consensus preliminary benchmark|P2MR SLH-DSA Rescue Leaf|Quantum-Rescue Signature|draft circulated as" "$BIP"; then
  echo "release_check.sh: stale BIP wording found" >&2
  exit 1
fi

grep -n "hypothesis to test" "$BIP"
grep -n "BIP-360 is still draft" "$BIP"
grep -n "Why a separate BIP rather than only BIP-360 text" "$BIP"
grep -n "P2MR/QRS transaction digest derived from the BIP-341" "$BIP"
grep -n "proposed script-tree output that removes the Taproot key-path spend" "$BIP"
grep -n "Fixed-length encoding and witness malleability" "$BIP"
grep -n "docs/evidence/resource-accounting-decision.md" "$BIP" "$START_HERE"
grep -n "docs/evidence/explicit-qrs-budget-fallback.md" "$BIP" "$ROOT/docs/evidence/resource-accounting-decision.md"
grep -n "docs/spec/bip360-dependency-matrix.md" "$ROOT/README.md" "$BIP" "$START_HERE"
grep -n "docs/evidence/consensus-gap-manifest.md" "$ROOT/README.md" "$START_HERE"
grep -n "docs/evidence/vector-coverage-matrix.md" "$START_HERE"
grep -n "docs/reproducibility/reproducibility.md" "$START_HERE"
grep -n "out/full.md" "$ROOT/README.md" "$START_HERE"
grep -n "Draft-stage" "$ROOT/README.md" "$START_HERE"
grep -n "not activation-ready" "$ROOT/README.md" "$START_HERE"
grep -n "not asking reviewers to support activation" "$START_HERE"
grep -n "No legacy-output" "$START_HERE"
grep -n "No tapscript opcode in v1" "$START_HERE"
grep -n "No witness discount" "$START_HERE"
grep -n "No algorithm registry" "$START_HERE"
grep -n "No activation parameters" "$START_HERE"
grep -n "Known Blockers Before Advancing Beyond Draft" "$START_HERE"
grep -n "Bitcoin Core consensus validation-path integration" "$START_HERE"
grep -n "Final serialized vectors remain blocked" "$START_HERE"
grep -n "BIP-341 field ordering" "$ROOT/docs/spec/final-transaction-vector-schema.md"
grep -n "qrs_transaction_vector.schema.json" "$ROOT/docs/spec/final-transaction-vector-schema.md"
grep -n "schema-only placeholder" "$ROOT/docs/spec/final-transaction-vector-schema.md"
grep -n "provisional OP_1/PUSH32" "$ROOT/docs/spec/final-transaction-vector-schema.md" "$ROOT/test_vectors/README.md"
grep -n "root-carrier encoding" "$ROOT/docs/spec/final-transaction-vector-schema.md" "$ROOT/test_vectors/README.md"
grep -n "Schema for future P2MR QRS serialized transaction vectors" "$ROOT/test_vectors/qrs_transaction_vector.schema.json"
grep -n "future_final_vector_contract" "$ROOT/test_vectors/qrs_transaction_vector.schema.json" "$ROOT/docs/spec/final-transaction-vector-schema.md"
grep -n "docs/evidence/consensus-gap-manifest.json" "$CONSENSUS_GAP_MANIFEST_MD" "$ROOT/docs/evidence/resource-accounting-decision.md"
grep -n "future leaf version behavior" "$ROOT/docs/spec/bip360-dependency-matrix.md"
grep -n "SigMsg extension behavior" "$ROOT/docs/spec/bip360-dependency-matrix.md"
grep -n "bip360_final_leaf_hashing" "$CONSENSUS_GAP_MANIFEST" "$CONSENSUS_GAP_MANIFEST_MD"
grep -n "qrs_ext_flag_assignment" "$CONSENSUS_GAP_MANIFEST" "$CONSENSUS_GAP_MANIFEST_MD"
grep -n "bitcoin_core_validation_path_integration" "$CONSENSUS_GAP_MANIFEST" "$CONSENSUS_GAP_MANIFEST_MD"
grep -n "reviewed_public_batch_schnorr_baseline" "$CONSENSUS_GAP_MANIFEST" "$CONSENSUS_GAP_MANIFEST_MD"
grep -n "docs/evidence/batch-schnorr-baseline-status.md" "$CONSENSUS_GAP_MANIFEST" "$CONSENSUS_GAP_MANIFEST_MD"
grep -n "reviewed public BIP-340 batch verification baseline unavailable" "$BATCH_SCHNORR_STATUS"
grep -n "not a reviewed public libsecp256k1 API" "$BATCH_SCHNORR_STATUS"
grep -n "must never synthesize a fake batch baseline" "$BATCH_SCHNORR_STATUS"
grep -n "final_serialized_consensus_vectors" "$CONSENSUS_GAP_MANIFEST" "$CONSENSUS_GAP_MANIFEST_MD"
grep -n "docs/reproducibility/reproduction-matrix.md" "$REPRODUCIBILITY"
grep -n "Apple Silicon macOS" "$ROOT/docs/reproducibility/reproduction-matrix.md"
grep -n "Linux x86_64" "$ROOT/docs/reproducibility/reproduction-matrix.md"
grep -n "minimum public-pre-review target" "$ROOT/docs/reproducibility/reproduction-matrix.md"
grep -n "pre-activation target" "$ROOT/docs/reproducibility/reproduction-matrix.md"
grep -n "docs/evidence/slh-dsa-verify-cost-analysis.md" "$ROOT/docs/evidence/resource-accounting-decision.md" "$REPRODUCIBILITY"
grep -n "empirical timing evidence" "$ROOT/docs/evidence/slh-dsa-verify-cost-analysis.md"
grep -n "algorithmic worst-case" "$ROOT/docs/evidence/slh-dsa-verify-cost-analysis.md"
grep -n "random bit flips are not a proof" "$ROOT/docs/evidence/slh-dsa-verify-cost-analysis.md"
grep -n "vector_coverage_matrix.json" "$ROOT/docs/evidence/vector-coverage-matrix.md" "$ROOT/test_vectors/README.md"
grep -n "vector_coverage_matrix.schema.json" "$ROOT/docs/evidence/vector-coverage-matrix.md" "$ROOT/scripts/validate_vector_coverage_matrix.py"
grep -n "P2MR QRS Vector Coverage Matrix" "$ROOT/test_vectors/vector_coverage_matrix.schema.json" "$ROOT/docs/evidence/vector-coverage-matrix.md"
grep -n "P2MR QRS Consensus Gap Manifest" "$ROOT/docs/evidence/consensus-gap-manifest.schema.json" "$ROOT/scripts/validate_consensus_gap_manifest.py"
grep -n "Final serialized consensus vectors remain blocked" "$ROOT/docs/evidence/vector-coverage-matrix.md"
grep -n "2.5x hypothetical reviewed-batch-Schnorr baseline" "$ROOT/docs/evidence/explicit-qrs-budget-fallback.md" "$ROOT/docs/evidence/resource-accounting-decision.md"
grep -n "reviewed public batch-Schnorr implementation becomes available" "$ROOT/docs/evidence/explicit-qrs-budget-fallback.md" "$ROOT/docs/evidence/resource-accounting-decision.md"
grep -n "fallback trigger" "$ROOT/docs/evidence/explicit-qrs-budget-fallback.md" "$ROOT/docs/evidence/resource-accounting-decision.md"
grep -Rni "blocking Draft pre-review" "$ROOT/.github/ISSUE_TEMPLATE"

if grep -RniE "test_vectors/.*are final consensus vectors|current test_vectors/.*are final consensus vectors" \
  "$ROOT/README.md" "$ROOT/docs" "$ROOT/test_vectors"; then
  echo "release_check.sh: current test_vectors/ must not be described as final consensus vectors" >&2
  exit 1
fi
if grep -Rni "consensus test vectors" "$ROOT/README.md" "$ROOT/docs" "$ROOT/test_vectors"; then
  echo "release_check.sh: vectors must not be described as consensus test vectors" >&2
  exit 1
fi
if grep -Rni "P2MR is the only key-path-free output" "$BIP" "$ROOT/README.md" "$ROOT/docs"; then
  echo "release_check.sh: avoid overbroad key-path-free-output claims" >&2
  exit 1
fi
if grep -RniE "QRS is [c]heaper|\\([c]heaper\\)|empirically supported" "$BIP" "$ROOT/README.md" "$ROOT/docs"; then
  echo "release_check.sh: unsupported cost language found" >&2
  exit 1
fi
ACTIVATION_READY_HITS="$(grep -Rni "activation-ready consensus evidence" "$ROOT/README.md" "$ROOT/docs" || true)"
ACTIVATION_READY_CLAIMS="$(printf "%s\n" "$ACTIVATION_READY_HITS" | grep -vi "not activation-ready consensus evidence" || true)"
if [ -n "$ACTIVATION_READY_CLAIMS" ]; then
  printf "%s\n" "$ACTIVATION_READY_CLAIMS"
  echo "release_check.sh: unsupported activation-ready consensus evidence claim found" >&2
  exit 1
fi
PROCESS_HITS="$(grep -RniE "Cl[a]ude|Co[d]ex|ChatG[P]T|A[I]-generated|\\bA[I]\\b|\\bL[L]M\\b|assist[a]nt" \
  --exclude-dir=.git --exclude-dir=build --exclude-dir=third_party --exclude-dir=__pycache__ \
  "$ROOT" || true)"
if [ -n "$PROCESS_HITS" ]; then
  printf "%s\n" "$PROCESS_HITS"
  echo "release_check.sh: non-technical process artifact wording found" >&2
  exit 1
fi

python3 "$ROOT/scripts/compute_qrs_digest_model.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/check_bip341_sigmsg_order.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/validate_vector_coverage_matrix.py"
python3 "$ROOT/scripts/validate_consensus_gap_manifest.py"
python3 "$ROOT/scripts/validate_test_vectors.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/verify_qrs_fixtures.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/run_qrs_negative_tests.py"

cmake -S "$ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" -j
if ! "$BUILD_DIR/qrs_native_bench" --slh-probe; then
  echo "release_check.sh: SLH-DSA backend probe failed; check OpenSSL version, provider loading, EVP_PKEY_CTX_new_from_name support for SLH-DSA-SHA2-128s, context-string support, and valid/mutated verification preflight" >&2
  exit 1
fi
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
python3 "$ROOT/scripts/check_batch_schnorr_baseline.py" "${BATCH_EVIDENCE_ARGS[@]}"
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
jq -e '.benchmarks.schnorr_bip340.individual_valid_verify.status == "available"' "$BATCH_DISABLED_JSON"
jq -e '.benchmarks.schnorr_bip340.individual_invalid_verify.status == "available"' "$BATCH_DISABLED_JSON"
jq -e '.benchmarks.schnorr_bip340.batch_experimental.status == "unavailable"' "$BATCH_DISABLED_JSON"
jq -e '.block_model.schnorr_individual_saturated_block.status == "available"' "$BATCH_DISABLED_JSON"
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
jq '.benchmarks.slh_dsa_sha2_128s.slh_dsa_backend_probe.keygen' "$TMP_QUICK_JSON"
jq '.benchmarks.slh_dsa_sha2_128s.slh_dsa_backend_probe.verify_mutated_signature' "$TMP_QUICK_JSON"
jq '.draft_rule_status' "$TMP_RESOURCE_DECISION_JSON"
jq '.activation_ready' "$TMP_RESOURCE_DECISION_JSON"
jq '.explicit_qrs_budget_required' "$TMP_RESOURCE_DECISION_JSON"
jq '.batch_sensitivity.status' "$TMP_RESOURCE_DECISION_JSON"
jq '.depth_sensitivity.qrs_depth0_p99_ms' "$TMP_RESOURCE_DECISION_JSON"
jq '.depth_sensitivity.qrs_depth128_p99_ms' "$TMP_RESOURCE_DECISION_JSON"
jq '.depth_sensitivity.qrs_depth_binding_case' "$TMP_RESOURCE_DECISION_JSON"
jq '.activation_blocker_manifest' "$TMP_RESOURCE_DECISION_JSON"
jq -e '.activation_blocker_ids | index("bitcoin_core_validation_path_integration")' "$TMP_RESOURCE_DECISION_JSON"
jq -e '.activation_blocker_ids | index("final_serialized_consensus_vectors")' "$TMP_RESOURCE_DECISION_JSON"
grep -n "Pass/Fail Conclusion" "$TMP_RESOURCE_DECISION_MD"
grep -n "QRS Depth Sensitivity" "$TMP_RESOURCE_DECISION_MD"
grep -n "Batch-Speedup Sensitivity" "$TMP_RESOURCE_DECISION_MD"
grep -n "Fallback Trigger Checks" "$TMP_RESOURCE_DECISION_MD"
grep -n "fallback trigger" "$TMP_RESOURCE_DECISION_MD"
grep -n "Hypothetical batch speedups are sensitivity analysis only" "$TMP_RESOURCE_DECISION_MD"
grep -n "does not establish activation readiness" "$TMP_RESOURCE_DECISION_MD"
grep -n "bitcoin_core_validation_path_integration" "$TMP_RESOURCE_DECISION_MD"
grep -n "docs/evidence/consensus-gap-manifest.json" "$TMP_RESOURCE_DECISION_MD"
grep -n "median of per-batch means" "$TMP_QUICK_MD"
grep -n "second reviewed SLH-DSA backend" "$TMP_QUICK_MD"
grep -n "explicit non-consensus prefixes" "$TMP_QUICK_MD"
grep -n "QRS modeled TapLeaf v0" "$TMP_QUICK_MD"
grep -n "BIP-340 challenge self-test" "$TMP_QUICK_MD"
ensure_out_clean

echo "release checklist passed"
