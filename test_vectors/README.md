# QRS Structured Fixture Vectors

These are deterministic v0.9.0 structured fixtures, not final consensus vectors.
They intentionally use provisional `QRS_LEAF_VERSION = 0xc2` and modeled
transaction fields until BIP-360 finalizes P2MR leaf hashing, branch hashing,
sighash tagging, annex handling, and future-leaf behavior.

The fixture hashes are executable under `scripts/verify_qrs_fixtures.py`, but
the signatures are descriptor bytes, not final SLH-DSA consensus signatures.
Native cryptographic verification is covered by the benchmark harness, not by
these provisional fixtures.

Each vector includes the fields required for final consensus-vector review:
structured transaction fields, spent output scriptPubKey, witness stack,
`qrs_public_key`, `qrs_signature`, `control_block`, `expected_leaf_hash`,
`expected_qrs_msg`, `expected_result`, and `failure_stage`.

The important invariant in this directory is failure-stage classification:
structural invalid cases must fail before SLH-DSA verification, while
`qrs_invalid_signature_fixed_length.json` must reach the verifier and fail
cryptographically.

Run:

```sh
python3 scripts/validate_test_vectors.py test_vectors/
python3 scripts/verify_qrs_fixtures.py test_vectors/
```
