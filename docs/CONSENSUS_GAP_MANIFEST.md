# Consensus Gap Manifest

This manifest is the machine-checked list of known gaps that block activation
or final consensus-vector claims for the P2MR SLH-DSA Leaf draft.

The canonical machine-readable file is
`docs/consensus-gap-manifest.json`. The schema is
`docs/consensus-gap-manifest.schema.json`, and
`scripts/validate_consensus_gap_manifest.py` verifies the manifest during the
release gate.

This is not a roadmap for activation. It is a fail-closed list of work that
must not be hidden by benchmark prose.

| Gap ID | Blocks | Current artifact |
| --- | --- | --- |
| `bip360_final_leaf_hashing` | activation, complete vectors | `docs/BIP360_DEPENDENCY_MATRIX.md` |
| `bip360_final_branch_hashing` | activation, complete vectors | `docs/BIP360_DEPENDENCY_MATRIX.md` |
| `bip360_final_future_leaf_behavior` | activation | `docs/BIP360_DEPENDENCY_MATRIX.md` |
| `qrs_ext_flag_assignment` | activation, complete vectors | `docs/BIP360_DEPENDENCY_MATRIX.md` |
| `final_sigmsg_definition` | activation, complete vectors | `docs/FINAL_TRANSACTION_VECTOR_SCHEMA.md` |
| `bitcoin_core_validation_path_integration` | activation, resource accounting | `docs/BITCOIN_CORE_INTEGRATION_REQUIREMENTS.md` |
| `second_reviewed_slh_dsa_backend` | activation | `docs/SLH_DSA_VERIFY_COST_ANALYSIS.md` |
| `cross_hardware_reproduction` | activation, resource accounting | `docs/REPRODUCTION_MATRIX.md` |
| `reviewed_public_batch_schnorr_baseline` | activation, resource accounting | `04_batch_schnorr_baseline.md` |
| `wallet_descriptor_psbt_hardware_standards` | activation, wallet standards | `docs/PUBLIC_REVIEW_READINESS.md` |
| `activation_parameters` | activation | `docs/PUBLIC_REVIEW_POST_DRAFT.md` |
| `final_serialized_consensus_vectors` | activation, complete vectors | `docs/FINAL_TRANSACTION_VECTOR_SCHEMA.md` |

## Release Rule

`bash scripts/release_check.sh` must validate the JSON manifest and must fail if
any required blocker ID is removed without updating the validator. Resource
accounting output must either include these blocker IDs or point to the
manifest, because `activation_ready = false` is not just prose.
