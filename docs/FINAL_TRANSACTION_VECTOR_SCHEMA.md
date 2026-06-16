# Final Transaction Vector Schema

Status: schema defined; final vectors blocked on final BIP-360/QRS definitions.

The current `test_vectors/` files are provisional executable structured
fixtures. They are stronger than metadata because verifier-reaching draft
cryptographic vectors carry real SLH-DSA-SHA2-128s signatures, but they are
still not final consensus vectors.

The provisional fixture model now covers the current BIP's non-provisional
preimage structure where possible: TapLeaf-style length-prefixed leaf payloads,
TapBranch-style Merkle paths, and a TapSighash-style message with a modeled
ext_flag=2 SigMsg payload followed by `leaf_hash`. What remains provisional is
the serialized transaction digest itself, because final BIP-360/QRS sighash
definitions are not yet fixed.

Final vectors must be serialized spend vectors. They must bind the exact bytes
that a consensus implementation validates.

## Final SigMsg blocker

Current executable vectors test the QRS preimage shape but do not claim final
Bitcoin Core `SigMsg` equivalence. Final consensus vectors require:

- finalized BIP-360/P2MR hashing rules;
- finalized QRS `ext_flag` assignment;
- Bitcoin Core-shaped transaction serialization;
- cross-implementation agreement on qrs_msg;
- serialized valid and invalid transactions.

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
