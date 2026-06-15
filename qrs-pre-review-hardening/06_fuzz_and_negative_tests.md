# Fuzz And Negative Tests

Artifacts:

- `../fuzz/qrs_witness_corpus/`
- `../tests/qrs_negative_cases.json`
- `../scripts/run_qrs_negative_tests.py`

Acceptance:

```sh
python3 ../scripts/run_qrs_negative_tests.py
grep -n "invalid_fixed_length_signature_reaches_verifier" ../tests/qrs_negative_cases.json
```
