#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIP="$ROOT/docs/bip-p2mr-quantum-rescue-leaf-v0.9.0.mediawiki"
QUICK_JSON="$ROOT/out/quick.json"
QUICK_MD="$ROOT/out/quick.md"
BATCH_EVIDENCE="$ROOT/out/batch-evidence.json"

grep -n "Version: 0.9.0" "$BIP"
grep -n "Requires: 341, 342, 360" "$BIP"
grep -n "hypothesis to test" "$BIP"
grep -n "BIP-360 is still draft" "$BIP"
grep -n "strong unforgeability" "$BIP"
grep -n "Draft-stage pre-review" "$ROOT/00_README.md"

if grep -RniE "QRS is [c]heaper|\\([c]heaper\\)|empirically supported" \
  "$BIP" "$ROOT/REVIEWER_DOSSIER.md" "$ROOT/RELEASE_CHECKLIST.md" "$ROOT/00_README.md"; then
  echo "release_check.sh: unsupported cost language found" >&2
  exit 1
fi

python3 "$ROOT/scripts/validate_test_vectors.py" "$ROOT/test_vectors"
python3 "$ROOT/scripts/run_qrs_negative_tests.py"

test -f "$QUICK_JSON"
test -f "$QUICK_MD"
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
