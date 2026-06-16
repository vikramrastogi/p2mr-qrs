# Reproducibility Protocol

This repository's local reports are not independent consensus evidence. A
reviewer rerun should produce raw artifacts that can be compared mechanically.

## Required Commands

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
python3 scripts/check_batch_schnorr_baseline.py \
  --json out/batch-evidence.json \
  --markdown out/batch-evidence.md
python3 scripts/verify_qrs_vectors.py test_vectors/ \
  --binary build/qrs_native_bench \
  --require-crypto
./build/qrs_native_bench --full \
  --json out/full.json \
  --markdown out/full.md
python3 scripts/assert_quick_report.py \
  --json out/full.json \
  --markdown out/full.md \
  --batch-evidence-json out/batch-evidence.json
python3 scripts/evaluate_resource_accounting.py \
  --report-json out/full.json \
  --batch-evidence-json out/batch-evidence.json \
  --json out/resource-accounting-decision.json \
  --markdown out/resource-accounting-decision.md
```

## Required Attachments

- `out/full.json`
- `out/full.md`
- `out/batch-evidence.json`
- `out/batch-evidence.md`
- `out/resource-accounting-decision.json`
- `out/resource-accounting-decision.md`

## Required Metadata

The generated report must identify:

- hardware;
- operating system;
- compiler and compiler flags;
- build mode;
- OpenSSL version and provider;
- SLH-DSA mode;
- libsecp256k1 commit;
- benchmark harness commit;
- working-tree dirty status.

## Interpretation

Independent reproduction can strengthen or falsify Draft-stage resource
accounting. It cannot by itself establish activation readiness unless the
remaining activation blockers are also closed: final BIP-360/QRS definitions,
Bitcoin Core validation-path integration, reviewed implementation selection,
and reviewer agreement on the batch-Schnorr baseline.
