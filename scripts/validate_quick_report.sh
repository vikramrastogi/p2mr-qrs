#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-"$ROOT/build"}"

cmake -S "$ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release

if [[ -n "${JOBS:-}" ]]; then
  cmake --build "$BUILD_DIR" -j "$JOBS"
else
  cmake --build "$BUILD_DIR" -j
fi

python3 "$ROOT/scripts/check_batch_schnorr_baseline.py" \
  --json "$ROOT/out/batch-evidence.json" \
  --markdown "$ROOT/out/batch-evidence.md" \
  "${QRS_BATCH_EVIDENCE_MODE:---skip-upstream}"

"$BUILD_DIR/qrs_native_bench" \
  --quick \
  --json "$ROOT/out/quick.json" \
  --markdown "$ROOT/out/quick.md"

python3 "$ROOT/scripts/assert_quick_report.py" \
  --json "$ROOT/out/quick.json" \
  --markdown "$ROOT/out/quick.md" \
  --batch-evidence-json "$ROOT/out/batch-evidence.json"

jq '.benchmarks.schnorr_bip340.batch.status' "$ROOT/out/quick.json"
jq '.benchmarks.schnorr_bip340.batch_experimental_valid.status' "$ROOT/out/quick.json"
jq '.benchmarks.schnorr_bip340.batch_experimental_challenge_self_test.status' "$ROOT/out/quick.json"
jq '.benchmarks.schnorr_bip340.batch_reviewed_public_api_status' "$ROOT/out/quick.json"
grep -n "experimental native BIP-340 batch baseline" "$ROOT/out/quick.md"
grep -n "BIP-340 challenge self-test" "$ROOT/out/quick.md"
grep -n "not a reviewed public libsecp256k1 API" "$ROOT/out/quick.md"
grep -n "QRS vs experimental batch Schnorr" "$ROOT/out/quick.md"
grep -n "reviewed public batch" "$ROOT/out/quick.md"
