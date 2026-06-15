# Experimental Batch Schnorr

Artifacts:

- `../src/experimental_batch_schnorr.c`
- `../include/qrs_bench/experimental_batch_schnorr.h`
- `../scripts/check_batch_schnorr_baseline.py`

Reviewed public batch API status remains separate from the experimental native
baseline.

Acceptance:

```sh
../build/qrs_native_bench --quick --json ../out/quick.json --markdown ../out/quick.md
jq '.benchmarks.schnorr_bip340.batch_reviewed_public_api.status' ../out/quick.json
jq '.benchmarks.schnorr_bip340.batch_experimental.status' ../out/quick.json
grep -n "experimental native BIP-340 batch baseline" ../out/quick.md
grep -n "not a reviewed public libsecp256k1 API" ../out/quick.md
```
