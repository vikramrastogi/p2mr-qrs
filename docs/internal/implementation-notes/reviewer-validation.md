# Reviewer Validation

Use this checklist before citing a run as native benchmark evidence.

## Quick Smoke

```sh
scripts/validate_quick_report.sh
```

This builds a Release binary, records batch-baseline evidence, runs the quick
benchmark, and asserts the JSON/Markdown contract. The batch-evidence step must
be able to query upstream `bitcoin-core/secp256k1`; if DNS or network access
fails or the upstream lookup exceeds its timeout, it reports
`evidence-incomplete`, writes the evidence report, and exits nonzero.

## Manual Acceptance

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
python3 scripts/check_batch_schnorr_baseline.py --json out/batch-evidence.json --markdown out/batch-evidence.md --upstream-timeout-seconds 15
./build/qrs_native_bench --quick --json out/quick.json --markdown out/quick.md
python3 scripts/assert_quick_report.py --json out/quick.json --markdown out/quick.md --batch-evidence-json out/batch-evidence.json
jq '.benchmarks.schnorr_bip340.batch_experimental_valid.status' out/quick.json
jq '.benchmarks.schnorr_bip340.batch_reviewed_public_api_status' out/quick.json
grep -n "experimental native BIP-340 batch baseline" out/quick.md
grep -n "not a reviewed public libsecp256k1 API" out/quick.md
grep -n "QRS vs experimental batch Schnorr" out/quick.md
grep -n "reviewed public batch" out/quick.md
```

## Publication Run

```sh
./build/qrs_native_bench --full --json out/full.json --markdown out/full.md
```

Before publishing, record the machine model, power mode, compiler, OpenSSL
version, libsecp256k1 commit, and benchmark commit. A single machine is still
not consensus-readiness evidence; it is one data point for review.
