# Native Benchmark Completion

Artifacts:

- `../src/slh_dsa_openssl.cpp`
- `../src/schnorr_secp256k1.cpp`
- `../src/report_json.cpp`
- `../src/report_markdown.cpp`
- `../out/quick.json`
- `../out/quick.md`

Acceptance:

```sh
cmake -S .. -B ../build -DCMAKE_BUILD_TYPE=Release
cmake --build ../build -j
../build/qrs_native_bench --quick --json ../out/quick.json --markdown ../out/quick.md
jq '.benchmarks.slh_dsa_sha2_128s.valid_verify.status' ../out/quick.json
jq '.benchmarks.schnorr_bip340.individual_valid_verify.status' ../out/quick.json
grep -n "median of per-batch means" ../out/quick.md
```
