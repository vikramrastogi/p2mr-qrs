# P2MR Quantum-Rescue Leaf

Draft-stage pre-review package for a narrow post-quantum signing primitive in
the P2MR / quantum-migration discussion.

This is not activation-ready consensus evidence.

## Scope

This repository proposes and tests one narrow primitive:

- one P2MR-only future leaf version;
- one SLH-DSA-SHA2-128s signature;
- no legacy-output freeze, burn, throttle, sunset, or rescue policy;
- no witness discount;
- no algorithm registry;
- no tapscript opcode in v1.

The design goal is the smallest useful post-quantum signing path that does not
require Bitcoin to resolve old-coin policy first.

## Main Unresolved Question

Resource accounting is the hard part.

The draft proposes no additional QRS validation budget beyond existing SegWit
weight accounting, but treats that rule as a hypothesis to test, not a
conclusion. Before this could advance beyond Draft, native review must cover
valid QRS spends, invalid fixed-length signatures, tail latency, implementation
variance, Bitcoin Core-shaped validation, and individual/batch Schnorr
baselines. If weight is insufficient, the draft requires an explicit per-QRS
validation budget before activation.

## Repository Map

- `docs/bip-p2mr-quantum-rescue-leaf-v0.9.0.mediawiki` - BIP draft.
- `docs/REVIEWER_DOSSIER.md` - predictable objections mapped to evidence,
  blockers, or non-goals.
- `docs/RELEASE_CHECKLIST.md` - posting and release checks.
- `src/` and `include/` - native benchmark harness.
- `scripts/` - validation, report, and batch-Schnorr evidence scripts.
- `test_vectors/` - provisional structured QRS consensus fixtures.
- `fuzz/` and `tests/` - malformed witness corpus and negative cases.
- `out/sample-native-report.md` - sample native benchmark report.
- `out/batch-evidence.md` - reviewed-public batch Schnorr status evidence.
- `third_party/secp256k1/` - vendored pinned libsecp256k1 source.
- `THIRD_PARTY_NOTICES.md` - third-party license notes.

## Quick Check

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/qrs_native_bench --quick --json out/quick.json --markdown out/quick.md
python3 scripts/assert_quick_report.py out/quick.json out/quick.md
python3 scripts/validate_test_vectors.py test_vectors/
python3 scripts/run_qrs_negative_tests.py
bash scripts/release_check.sh
```

`scripts/check_batch_schnorr_baseline.py` can refresh the batch-Schnorr evidence
report. Use `--skip-upstream` for local-only checks; omit it when publishing
network-checked upstream evidence.

## Current Status

- Native OpenSSL SLH-DSA-SHA2-128s timing is measured where the local OpenSSL
  build exposes the algorithm.
- Native libsecp256k1 individual BIP-340 Schnorr timing is measured.
- Experimental native BIP-340 batch timing is included for sensitivity analysis
  only.
- Reviewed public libsecp256k1 batch verification remains explicitly
  unavailable in this package.
- The QRS validation-path section is a model, not Bitcoin Core consensus
  integration.

Known blockers before advancing beyond Draft are listed in
`docs/REVIEWER_DOSSIER.md`.
