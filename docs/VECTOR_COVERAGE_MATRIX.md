# Vector Coverage Matrix

This matrix makes fixture coverage explicit. It is not a final consensus-vector
claim.

The machine-readable matrix is
`test_vectors/vector_coverage_matrix.json`. Its schema metadata is locked in
`test_vectors/vector_coverage_matrix.schema.json`, whose title is
`P2MR QRS Vector Coverage Matrix`. The release gate validates both files with
`scripts/validate_vector_coverage_matrix.py`.

Current coverage is split across:

- provisional executable structured fixtures in `test_vectors/`;
- negative/fuzz witness cases in `tests/` and `fuzz/`;
- digest mutation self-tests in `scripts/compute_qrs_digest_model.py`;
- fixed-length invalid verifier cases in the native SLH-DSA benchmark.

Final serialized consensus vectors remain blocked on final BIP-360/QRS hashing
and sighash definitions.

| Case ID | Artifact | Expected | Failure stage | Reaches verifier |
| --- | --- | --- | --- | --- |
| `valid_no_annex` | `test_vectors/qrs_valid_no_annex.json` | true | `none` | true |
| `valid_with_annex` | `test_vectors/qrs_valid_with_annex.json` | true | `none` | true |
| `invalid_fixed_length_signature` | `test_vectors/qrs_invalid_signature_fixed_length.json` | false | `slh_dsa_verify` | true |
| `invalid_public_key_length` | `test_vectors/qrs_invalid_pubkey_length.json` | false | `public_key_length` | false |
| `invalid_signature_length` | `test_vectors/qrs_invalid_signature_length.json` | false | `signature_length` | false |
| `invalid_control_block_length` | `test_vectors/qrs_invalid_control_block_length.json` | false | `control_block_length` | false |
| `control_low_bit_unset` | `test_vectors/qrs_control_low_bit_unset.json` | false | `control_low_bit` | false |
| `invalid_leaf_version` | `test_vectors/qrs_invalid_leaf_version.json` | false | `leaf_version` | false |
| `merkle_root_mismatch` | `test_vectors/qrs_merkle_root_mismatch.json` | false | `merkle_root_mismatch` | false |
| `malformed_witness_missing_element` | `fuzz/qrs_witness_corpus/too_few_witness_elements.json` | false | `malformed_witness` | false |
| `malformed_witness_extra_element` | `test_vectors/qrs_malformed_witness_extra_element.json` | false | `malformed_witness` | false |
| `malformed_witness_extra_after_annex` | `test_vectors/qrs_malformed_witness_extra_after_annex.json` | false | `malformed_witness` | false |
| `maximum_depth_merkle_path` | `test_vectors/qrs_max_depth_path.json` | true | `none` | true |
| `multi_input_distinct_spent_outputs` | `test_vectors/qrs_multi_input_explicit_spent_outputs.json` | false | `merkle_root_mismatch` | false |
| `annex_changes_qrs_msg` | `scripts/compute_qrs_digest_model.py` | true | `qrs_msg_changed` | false |
| `output_amount_changes_qrs_msg` | `scripts/compute_qrs_digest_model.py` | true | `qrs_msg_changed` | false |
| `spent_output_amount_changes_qrs_msg` | `scripts/compute_qrs_digest_model.py` | true | `qrs_msg_changed` | false |
| `spent_output_script_changes_qrs_msg` | `scripts/compute_qrs_digest_model.py` | true | `qrs_msg_changed` | false |
| `leaf_pubkey_changes_qrs_msg` | `scripts/compute_qrs_digest_model.py` | true | `qrs_msg_changed` | false |
| `wrong_message_signature_fails` | `src/slh_dsa_openssl.cpp` | false | `slh_dsa_verify` | true |
| `wrong_public_key_signature_fails` | `src/slh_dsa_openssl.cpp` | false | `slh_dsa_verify` | true |

The validator checks that every artifact exists and that fixture/fuzz metadata
matches the matrix. For digest and benchmark-source rows, it checks the named
self-test labels remain present in the referenced source.
