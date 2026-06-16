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
BUILD_DIR="$ROOT/build"

grep -n "Version: 0.9.0" "$BIP"
grep -n "Requires: 341, 342, 360" "$BIP"
grep -n "hypothesis to test" "$BIP"
grep -n "BIP-360 is still draft" "$BIP"
grep -n "strong unforgeability" "$BIP"
grep -n "Draft-stage pre-review" "$ROOT/00_README.md"
grep -n "Draft-stage pre-review" "$ROOT/README.md"

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
"$BUILD_DIR/qrs_native_bench" --quick --json "$QUICK_JSON" --markdown "$QUICK_MD"
cp "$QUICK_MD" "$SAMPLE_MD"

python3 "$ROOT/scripts/assert_quick_report.py" \
  --json "$QUICK_JSON" \
  --markdown "$QUICK_MD" \
  --batch-evidence-json "$BATCH_EVIDENCE"

jq '.benchmarks.slh_dsa_sha2_128s.valid_verify.status' "$QUICK_JSON"
jq '.benchmarks.schnorr_bip340.individual_valid_verify.status' "$QUICK_JSON"
jq '.benchmarks.schnorr_bip340.batch_reviewed_public_api.status' "$QUICK_JSON"
jq '.benchmarks.schnorr_bip340.batch_experimental.status' "$QUICK_JSON"
jq '.benchmarks.qrs_validation_path.total_valid.status' "$QUICK_JSON"
grep -n "median of per-batch means" "$QUICK_MD"

echo "release checklist passed"
