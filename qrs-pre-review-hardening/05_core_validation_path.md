# Core Validation Path Model

Artifacts:

- `../src/core_validation_model.cpp`
- `../include/qrs_bench/core_validation_model.h`
- `../docs/core-validation-model.md`

This is a modeled validation path, not Bitcoin Core integration.

Acceptance:

```sh
../build/qrs_native_bench --quick --json ../out/quick.json --markdown ../out/quick.md
jq '.benchmarks.qrs_validation_path.total_valid.status' ../out/quick.json
jq '.benchmarks.qrs_validation_path.invalid_structural.status' ../out/quick.json
jq '.benchmarks.qrs_validation_path.invalid_fixed_length.status' ../out/quick.json
```
