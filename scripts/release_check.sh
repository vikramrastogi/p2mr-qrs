#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIP="$ROOT/docs/bip-p2mr-quantum-rescue-leaf-v0.9.0.mediawiki"
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
SECP_COMMIT_FILE="$ROOT/third_party/secp256k1.COMMIT"

grep -n "Version: 0.9.0" "$BIP"
grep -n "Requires: 341, 342, 360" "$BIP"
grep -n "Title: P2MR SLH-DSA Leaf" "$BIP"
grep -n "Authors: Vikram Rastogi <vikramrastogi@gmail.com>" "$BIP"
grep -n "Assigned: ?" "$BIP"
STALE_TITLE_PATTERN="Title: P2MR Quantum-""Rescue Leaf"
if grep -nE "Author:|Created:|$STALE_TITLE_PATTERN" "$BIP"; then
  echo "release_check.sh: stale BIP preamble field found" >&2
  exit 1
fi
grep -n "hypothesis to test" "$BIP"
grep -n "BIP-360 is still draft" "$BIP"
grep -n "strong unforgeability" "$BIP"
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
PRIVATE_SCAFFOLD_A="$ROOT/co""dex-specs"
PRIVATE_SCAFFOLD_B="$ROOT/qrs-pre-review-hardening"
if [ -e "$PRIVATE_SCAFFOLD_A" ] || [ -e "$PRIVATE_SCAFFOLD_B" ]; then
  echo "release_check.sh: private scaffolding directories must not be published" >&2
  exit 1
fi
if grep -RniE "Cl[a]ude|Co[d]ex|ChatG[P]T|A[I]-generated|\\bA[I]\\b|\\bL[L]M\\b|assist[a]nt" \
  --exclude-dir=.git --exclude-dir=build --exclude-dir=third_party \
  --exclude=.git \
  "$ROOT"; then
  echo "release_check.sh: non-technical process artifact wording found" >&2
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

python3 "$ROOT/scripts/validate_test_vectors.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/verify_qrs_fixtures.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/run_qrs_negative_tests.py"

cmake -S "$ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" -j
python3 "$ROOT/scripts/check_batch_schnorr_baseline.py" \
  --json "$BATCH_EVIDENCE" \
  --markdown "$BATCH_EVIDENCE_MD" \
  --upstream-timeout-seconds 15
python3 "$ROOT/scripts/verify_qrs_vectors.py" \
  "$ROOT/test_vectors" \
  --binary "$BUILD_DIR/qrs_native_bench" \
  --require-crypto
"$BUILD_DIR/qrs_native_bench" --quick --json "$QUICK_JSON" --markdown "$QUICK_MD"
cp "$QUICK_MD" "$SAMPLE_MD"

CURRENT_COMMIT="$(git -C "$ROOT" rev-parse HEAD)"
REPORT_COMMIT="$(jq -r '.environment.git_commit' "$QUICK_JSON")"
REPORT_DIRTY="$(jq -r '.environment.working_tree_dirty' "$QUICK_JSON")"
if [ "$REPORT_COMMIT" != "$CURRENT_COMMIT" ]; then
  echo "release_check.sh: quick report commit $REPORT_COMMIT does not match HEAD $CURRENT_COMMIT" >&2
  exit 1
fi
if [ "$REPORT_DIRTY" != "false" ]; then
  echo "release_check.sh: quick report was generated from a dirty source tree" >&2
  exit 1
fi

EXPECTED_SECP_COMMIT="$(tr -d '[:space:]' < "$SECP_COMMIT_FILE")"
REPORT_SECP_COMMIT="$(jq -r '.benchmarks.schnorr_bip340.libsecp256k1_commit' "$QUICK_JSON")"
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
  --json "$QUICK_JSON" \
  --markdown "$QUICK_MD" \
  --batch-evidence-json "$BATCH_EVIDENCE"
python3 "$ROOT/scripts/evaluate_resource_accounting.py" \
  --report-json "$QUICK_JSON" \
  --batch-evidence-json "$BATCH_EVIDENCE" \
  --json "$RESOURCE_DECISION_JSON" \
  --markdown "$RESOURCE_DECISION_MD" \
  --advisory

jq '.benchmarks.slh_dsa_sha2_128s.valid_verify.status' "$QUICK_JSON"
jq '.benchmarks.schnorr_bip340.individual_valid_verify.status' "$QUICK_JSON"
jq '.benchmarks.schnorr_bip340.batch_reviewed_public_api.status' "$QUICK_JSON"
jq '.benchmarks.schnorr_bip340.batch_experimental.status' "$QUICK_JSON"
jq '.benchmarks.schnorr_bip340.batch_experimental_challenge_self_test.status' "$QUICK_JSON"
jq '.benchmarks.qrs_validation_path.total_valid.status' "$QUICK_JSON"
jq '.environment.git_commit' "$QUICK_JSON"
jq '.environment.working_tree_dirty' "$QUICK_JSON"
jq '.environment.benchmark_binary_build_mode' "$QUICK_JSON"
jq '.draft_rule_status' "$RESOURCE_DECISION_JSON"
jq '.activation_ready' "$RESOURCE_DECISION_JSON"
jq '.explicit_qrs_budget_required' "$RESOURCE_DECISION_JSON"
grep -n "Pass/Fail Conclusion" "$RESOURCE_DECISION_MD"
grep -n "median of per-batch means" "$QUICK_MD"
grep -n "BIP-340 challenge self-test" "$QUICK_MD" "$DOSSIER"

echo "release checklist passed"
