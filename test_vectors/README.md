# QRS Provisional Executable Structured Fixtures

These are deterministic v0.9.0 provisional executable structured fixtures, not
final consensus vectors. They intentionally use provisional
`QRS_LEAF_VERSION = 0xc2` and modeled transaction fields until BIP-360
finalizes P2MR leaf hashing, branch hashing, sighash tagging, annex handling,
and future-leaf behavior.

The fixture hashes are executable under `scripts/verify_qrs_fixtures.py`.
Verifier-reaching draft cryptographic vectors additionally carry real
fixed-length SLH-DSA-SHA2-128s signatures checked by
`scripts/verify_qrs_vectors.py` through the native benchmark binary. The
vectors are still provisional because the hashing model is provisional pending
final BIP-360/QRS definitions.

The provisional model intentionally follows the current BIP's structural
preimage shape where it can: leaf hashes use
`TapLeaf(version || compact_size(len(pubkey)) || pubkey)`, branch hashes use
`TapBranch`, and message hashes use
`TapSighash(0x00 || modeled_sigmsg || leaf_hash)` with an ext_flag=2
spend-type byte in the BIP-341 input-data position after the common transaction
hashes. The modeled SigMsg is not a final BIP-341 transaction digest; it is a
deterministic fixture payload until the final BIP-360/QRS digest rules exist.
The `spent_output_scriptPubKey` values use a provisional OP_1/PUSH32
root-carrier encoding (`5120 || root`) only so the fixtures have an executable
32-byte root commitment; this is not a claim about final BIP-360 P2MR output
encoding.

Each vector includes the fields required for final consensus-vector review:
structured transaction fields, spent output scriptPubKey, witness stack,
`qrs_public_key`, `qrs_signature`, `control_block`, `expected_leaf_hash`,
`expected_qrs_msg`, `expected_result`, and `failure_stage`.

The final serialized transaction-vector schema is defined in
`qrs_transaction_vector.schema.json` and documented in
`../docs/FINAL_TRANSACTION_VECTOR_SCHEMA.md`. It is not populated yet because
the final BIP-360/QRS sighash and hashing definitions do not exist yet.

The provisional coverage matrix is `vector_coverage_matrix.json` and is
documented in `../docs/VECTOR_COVERAGE_MATRIX.md`. It maps expected reviewer
coverage to structured fixtures, negative/fuzz cases, digest mutation
self-tests, and invalid-verifier benchmark cases.

The important invariant in this directory is failure-stage classification:
structural invalid cases must fail before SLH-DSA verification, while
`qrs_invalid_signature_fixed_length.json` must reach the verifier and fail
cryptographically.

Run:

```sh
python3 scripts/validate_test_vectors.py test_vectors/
python3 scripts/validate_vector_coverage_matrix.py
python3 scripts/verify_qrs_fixtures.py test_vectors/
python3 scripts/verify_qrs_vectors.py test_vectors/ --binary build/qrs_native_bench
```
