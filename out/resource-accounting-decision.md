# Resource Accounting Evaluation

## Pass/Fail Conclusion

- `draft_rule_status`: explicit_budget_required_before_activation
- `activation_ready`: false
- `explicit_qrs_budget_required`: true
- `fallback_budget_status`: required_before_activation
- `conclusion`: QRS worst-case p99 exceeds individual Schnorr; the draft must add an explicit per-QRS validation budget before activation.

## Compared Estimates

| Estimate | p99 ms |
| --- | ---: |
| qrs_valid_saturated_block | 96.064 |
| qrs_worst_invalid_fixed_length_saturated_block | 398.965 |
| individual_schnorr_saturated_block | 364.410 |
| experimental_batch_schnorr_saturated_block | 243.326 |
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
| QRS worst p99 below individual Schnorr p99 | fail | qrs_worst=398.965 ms, individual_schnorr=364.410 ms |
| QRS worst p99 below experimental batch Schnorr p99 | fail | qrs_worst=398.965 ms, experimental_batch=243.326 ms |
| Experimental batch tightens individual Schnorr baseline | pass | experimental_batch=243.326 ms, individual_schnorr=364.410 ms |

## Batch-Speedup Sensitivity

Hypothetical batch speedups are sensitivity analysis only; they are not a measured baseline and no fake batch baseline is synthesized.

If the experimental batch baseline is slower than individual Schnorr, it is reported as non-tightening and cannot support the no-additional-budget rule for activation review.

- Experimental batch tightens individual Schnorr: true
- Experimental batch speedup vs individual: 1.498x
- QRS valid exceeds a hypothetical batch baseline if speedup is greater than 3.793x.
- QRS worst-invalid exceeds a hypothetical batch baseline if speedup is greater than 0.913x.

| Hypothetical speedup vs individual | Hypothetical Schnorr p99 ms | QRS valid exceeds | QRS worst-invalid exceeds |
| ---: | ---: | --- | --- |
| 1.5x | 242.940 | false | true |
| 2.0x | 182.205 | false | true |
| 2.5x | 145.764 | false | true |
| 3.0x | 121.470 | false | true |
| 4.0x | 91.103 | true | true |
| 5.0x | 72.882 | true | true |

## Fallback Trigger Checks

Fallback trigger checks identify when the inactive explicit QRS budget must be reconsidered before activation.

| Fallback trigger | Triggered | Threshold p99 ms | QRS worst p99 ms | Detail |
| --- | --- | ---: | ---: | --- |
| qrs_worst_exceeds_individual_schnorr | true | 364.410 | 398.965 | fallback trigger if QRS worst-invalid p99 exceeds individual Schnorr p99 |
| qrs_worst_exceeds_2_5x_hypothetical_reviewed_batch_schnorr | true | 145.764 | 398.965 | fallback trigger if QRS worst-invalid p99 exceeds a 2.5x hypothetical reviewed-batch-Schnorr baseline |
| qrs_worst_exceeds_3_0x_hypothetical_reviewed_batch_schnorr | true | 121.470 | 398.965 | fallback trigger if QRS worst-invalid p99 exceeds a 3.0x hypothetical reviewed-batch-Schnorr baseline |
| qrs_worst_exceeds_reviewed_public_batch_schnorr | unavailable | unavailable | 398.965 | fallback trigger if a reviewed public batch-Schnorr implementation becomes available and QRS exceeds it |
| bitcoin_core_validation_path_overhead | unavailable | unavailable | 398.965 | fallback trigger requires Bitcoin Core validation-path integration evidence; current harness models that path only |

## Activation Blockers

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
