# Resource Accounting Evaluation

## Pass/Fail Conclusion

- `draft_rule_status`: supports_no_additional_budget_for_draft_review
- `activation_ready`: false
- `explicit_qrs_budget_required`: false
- `fallback_budget_status`: not_required_by_current_draft_evidence
- `conclusion`: Current native evidence supports continuing with the no-additional-budget rule as Draft-stage review material.

## Compared Estimates

| Estimate | p99 ms |
| --- | ---: |
| qrs_valid_saturated_block | 112.748 |
| qrs_worst_invalid_fixed_length_saturated_block | 100.367 |
| individual_schnorr_saturated_block | 417.070 |
| experimental_batch_schnorr_saturated_block | 259.391 |
| reviewed_public_batch_schnorr_saturated_block | unavailable |

## Checks

| Check | Result | Detail |
| --- | --- | --- |
| SLH-DSA timings available | pass | available |
| Invalid fixed-length SLH-DSA timings available | pass | available |
| QRS validation-path model available | pass | available |
| Individual Schnorr baseline available | pass | available |
| Reviewed public batch status explicit | pass | unavailable |
| Batch evidence forbids fake speedups | pass | No batch Schnorr timing is synthesized from assumed speedups. |
| Report metadata complete | pass | complete |
| QRS worst p99 below individual Schnorr p99 | pass | qrs_worst=112.748 ms, individual_schnorr=417.070 ms |
| QRS worst p99 below experimental batch Schnorr p99 | pass | qrs_worst=112.748 ms, experimental_batch=259.391 ms |
| Experimental batch tightens individual Schnorr baseline | pass | experimental_batch=259.391 ms, individual_schnorr=417.070 ms |

## QRS Depth Sensitivity

| Field | Value |
| --- | ---: |
| qrs_depth0_p99_ms | 112.748 |
| qrs_depth128_p99_ms | 74.786 |
| qrs_depth_binding_case | depth128_lower_block_time_due_to_fewer_weight_limited_inputs |

Depth-sensitive rows count Merkle path witness weight and TapBranch hash counts. They do not benchmark Bitcoin Core branch-hash implementation overhead.


## Batch-Speedup Sensitivity

Hypothetical batch speedups are sensitivity analysis only; they are not a measured baseline and no fake batch baseline is synthesized.

If the experimental batch baseline is slower than individual Schnorr, it is reported as non-tightening and cannot support the no-additional-budget rule for activation review.

- Experimental batch tightens individual Schnorr: true
- Experimental batch speedup vs individual: 1.608x
- QRS valid exceeds a hypothetical batch baseline if speedup is greater than 3.699x.
- QRS worst-invalid exceeds a hypothetical batch baseline if speedup is greater than 3.699x.

| Hypothetical speedup vs individual | Hypothetical Schnorr p99 ms | QRS valid exceeds | QRS worst-invalid exceeds |
| ---: | ---: | --- | --- |
| 1.5x | 278.047 | false | false |
| 2.0x | 208.535 | false | false |
| 2.5x | 166.828 | false | false |
| 3.0x | 139.023 | false | false |
| 4.0x | 104.267 | true | true |
| 5.0x | 83.414 | true | true |

## Fallback Trigger Checks

Fallback trigger checks identify when the inactive explicit QRS budget must be reconsidered before activation.

| Fallback trigger | Triggered | Threshold p99 ms | QRS worst p99 ms | Detail |
| --- | --- | ---: | ---: | --- |
| qrs_worst_exceeds_individual_schnorr | false | 417.070 | 112.748 | fallback trigger if QRS worst-invalid p99 exceeds individual Schnorr p99 |
| qrs_worst_exceeds_2_5x_hypothetical_reviewed_batch_schnorr | false | 166.828 | 112.748 | fallback trigger if QRS worst-invalid p99 exceeds a 2.5x hypothetical reviewed-batch-Schnorr baseline |
| qrs_worst_exceeds_3_0x_hypothetical_reviewed_batch_schnorr | false | 139.023 | 112.748 | fallback trigger if QRS worst-invalid p99 exceeds a 3.0x hypothetical reviewed-batch-Schnorr baseline |
| qrs_worst_exceeds_reviewed_public_batch_schnorr | unavailable | unavailable | 112.748 | fallback trigger if a reviewed public batch-Schnorr implementation becomes available and QRS exceeds it |
| bitcoin_core_validation_path_overhead | unavailable | unavailable | 112.748 | fallback trigger requires Bitcoin Core validation-path integration evidence; current harness models that path only |

## Activation Blockers

- Manifest: `docs/consensus-gap-manifest.json`
- `bip360_final_leaf_hashing`
- `bip360_final_branch_hashing`
- `bip360_final_future_leaf_behavior`
- `qrs_ext_flag_assignment`
- `final_sigmsg_definition`
- `bitcoin_core_validation_path_integration`
- `second_reviewed_slh_dsa_backend`
- `cross_hardware_reproduction`
- `reviewed_public_batch_schnorr_baseline`
- `wallet_descriptor_psbt_hardware_standards`
- `activation_parameters`
- `final_serialized_consensus_vectors`

- Reviewed public batch Schnorr baseline unavailable.
- Bitcoin Core validation-path integration not implemented.
- Final BIP-360/QRS transaction digest definitions not available.
- Independent cross-hardware reproduction not attached.
- Second reviewed SLH-DSA implementation not wired.

This generated file does not establish activation readiness.
It is release evidence, not consensus activation evidence.
Activation still requires final BIP-360/QRS definitions, Bitcoin Core
validation-path integration, independent reproduction, implementation review,
and reviewer agreement on the batch-Schnorr baseline question.
