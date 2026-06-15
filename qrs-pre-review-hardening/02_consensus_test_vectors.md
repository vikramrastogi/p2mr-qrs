# Consensus Test Vectors

Artifacts:

- `../test_vectors/`
- `../scripts/validate_test_vectors.py`

The vectors are provisional structured fixtures pending final BIP-360/QRS
hashing and sighash definitions.

Acceptance:

```sh
python3 ../scripts/validate_test_vectors.py ../test_vectors/
jq '.expected_result' ../test_vectors/qrs_valid_no_annex.json
jq '.failure_stage' ../test_vectors/qrs_invalid_signature_fixed_length.json
```
