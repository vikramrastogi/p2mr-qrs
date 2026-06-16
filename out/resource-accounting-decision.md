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
| qrs_valid_saturated_block | 116.209 |
| qrs_worst_invalid_fixed_length_saturated_block | 123.575 |
| individual_schnorr_saturated_block | 451.535 |
| experimental_batch_schnorr_saturated_block | 242.634 |
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
| QRS worst p99 below individual Schnorr p99 | pass | qrs_worst=123.575 ms, individual_schnorr=451.535 ms |
| QRS worst p99 below experimental batch Schnorr p99 | pass | qrs_worst=123.575 ms, experimental_batch=242.634 ms |
| Experimental batch tightens individual Schnorr baseline | pass | experimental_batch=242.634 ms, individual_schnorr=451.535 ms |

## Batch-Speedup Sensitivity

Hypothetical batch speedups are sensitivity analysis only; they are not a measured baseline and no fake batch baseline is synthesized.

If the experimental batch baseline is slower than individual Schnorr, it is reported as non-tightening and cannot support the no-additional-budget rule for activation review.

- Experimental batch tightens individual Schnorr: true
- Experimental batch speedup vs individual: 1.861x
- QRS valid exceeds a hypothetical batch baseline if speedup is greater than 3.886x.
- QRS worst-invalid exceeds a hypothetical batch baseline if speedup is greater than 3.654x.

| Hypothetical speedup vs individual | Hypothetical Schnorr p99 ms | QRS valid exceeds | QRS worst-invalid exceeds |
| ---: | ---: | --- | --- |
| 1.5x | 301.023 | false | false |
| 2.0x | 225.768 | false | false |
| 2.5x | 180.614 | false | false |
| 3.0x | 150.512 | false | false |
| 4.0x | 112.884 | true | true |
| 5.0x | 90.307 | true | true |

## Fallback Trigger Checks

Fallback trigger checks identify when the inactive explicit QRS budget must be reconsidered before activation.

| Fallback trigger | Triggered | Threshold p99 ms | QRS worst p99 ms | Detail |
| --- | --- | ---: | ---: | --- |
| qrs_worst_exceeds_individual_schnorr | false | 451.535 | 123.575 | fallback trigger if QRS worst-invalid p99 exceeds individual Schnorr p99 |
| qrs_worst_exceeds_2_5x_hypothetical_reviewed_batch_schnorr | false | 180.614 | 123.575 | fallback trigger if QRS worst-invalid p99 exceeds a 2.5x hypothetical reviewed-batch-Schnorr baseline |
| qrs_worst_exceeds_3_0x_hypothetical_reviewed_batch_schnorr | false | 150.512 | 123.575 | fallback trigger if QRS worst-invalid p99 exceeds a 3.0x hypothetical reviewed-batch-Schnorr baseline |
| qrs_worst_exceeds_reviewed_public_batch_schnorr | unavailable | unavailable | 123.575 | fallback trigger if a reviewed public batch-Schnorr implementation becomes available and QRS exceeds it |
| bitcoin_core_validation_path_overhead | unavailable | unavailable | 123.575 | fallback trigger requires Bitcoin Core validation-path integration evidence; current harness models that path only |

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
