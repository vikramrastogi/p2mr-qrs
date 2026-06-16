# Final Transaction Vector Schema

Status: schema defined; final vectors blocked on final BIP-360/QRS definitions.

The current `test_vectors/` files are provisional executable structured
fixtures. They are stronger than metadata because verifier-reaching draft
cryptographic vectors carry real SLH-DSA-SHA2-128s signatures, but they are
still not final consensus vectors.

Final vectors must be serialized spend vectors. They must bind the exact bytes
that a consensus implementation validates.

## Required Vector Fields

Each final vector must include:

- `name`
- `bip360_revision`
- `qrs_bip_revision`
- `raw_spending_transaction_hex`
- `input_index`
- `spent_outputs`
- `spent_output_scriptPubKey`
- `witness_stack`
- `annex`
- `qrs_leaf_version`
- `qrs_ext_flag`
- `qrs_hash_type`
- `qrs_public_key`
- `qrs_signature`
- `control_block`
- `leaf_hash`
- `qrs_msg`
- `expected_result`
- `failure_stage`
- `reaches_slh_dsa_verifier`

The machine-readable schema lives at
`test_vectors/qrs_transaction_vector.schema.json`.

## Required Cases

The final vector set must include:

- valid QRS spend without annex;
- valid QRS spend with annex;
- invalid fixed-length signature that reaches the SLH-DSA verifier;
- invalid public-key length;
- invalid signature length;
- invalid control-block length;
- control byte with low bit unset;
- invalid leaf version;
- Merkle-root mismatch;
- malformed witness stack;
- maximum-depth Merkle path;
- cross-implementation transaction-digest agreement.

## Release Rule

Until final BIP-360/QRS hashing and sighash definitions are available, this
repository must label vectors as provisional executable structured fixtures or
draft cryptographic vectors, depending on the artifact. When final definitions
exist, release checks should add serialized-vector validation and the BIP should
stop referring to the vector set as provisional.
