# Consensus Gap Manifest

This manifest is the machine-checked list of known gaps that block activation
or final consensus-vector claims for the P2MR SLH-DSA Leaf draft.

The canonical machine-readable file is
`docs/evidence/consensus-gap-manifest.json`. The schema is
`docs/evidence/consensus-gap-manifest.schema.json`, and
`scripts/validate_consensus_gap_manifest.py` verifies the manifest during the
release gate.

This is not a roadmap for activation. It is a fail-closed list of work that
must not be hidden by benchmark prose.

| Gap ID | Blocks | Current artifact |
| --- | --- | --- |
| `bip360_final_leaf_hashing` | activation, complete vectors | `docs/spec/bip360-dependency-matrix.md` |
| `bip360_final_branch_hashing` | activation, complete vectors | `docs/spec/bip360-dependency-matrix.md` |
| `bip360_final_future_leaf_behavior` | activation | `docs/spec/bip360-dependency-matrix.md` |
| `qrs_ext_flag_assignment` | activation, complete vectors | `docs/spec/bip360-dependency-matrix.md` |
| `final_sigmsg_definition` | activation, complete vectors | `docs/spec/final-transaction-vector-schema.md` |
| `bitcoin_core_validation_path_integration` | activation, resource accounting | `docs/evidence/bitcoin-core-integration-requirements.md` |
| `second_reviewed_slh_dsa_backend` | activation | `docs/evidence/slh-dsa-verify-cost-analysis.md` |
| `cross_hardware_reproduction` | activation, resource accounting | `docs/reproducibility/reproduction-matrix.md` |
| `reviewed_public_batch_schnorr_baseline` | activation, resource accounting | `docs/evidence/batch-schnorr-baseline-status.md` |
| `wallet_descriptor_psbt_hardware_standards` | activation, wallet standards | `docs/review/start-here.md` |
| `activation_parameters` | activation | `docs/review/start-here.md` |
| `final_serialized_consensus_vectors` | activation, complete vectors | `docs/spec/final-transaction-vector-schema.md` |

## Release Rule

`bash scripts/release_check.sh` must validate the JSON manifest and must fail if
any required blocker ID is removed without updating the validator. Resource
accounting output must either include these blocker IDs or point to the
manifest, because `activation_ready = false` is not just prose.
