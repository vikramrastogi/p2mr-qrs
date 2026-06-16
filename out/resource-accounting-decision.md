# Resource Accounting Evaluation

## Pass/Fail Conclusion

- `draft_rule_status`: supports_no_additional_budget_for_draft
- `activation_ready`: false
- `explicit_qrs_budget_required`: false
- `fallback_budget_status`: not_required_by_current_draft_evidence
- `conclusion`: Current native evidence supports continuing with the no-additional-budget rule as Draft-stage review material.

## Compared Estimates

| Estimate | p99 ms |
| --- | ---: |
| qrs_valid_saturated_block | 90.887 |
| qrs_worst_invalid_fixed_length_saturated_block | 95.910 |
| individual_schnorr_saturated_block | 345.599 |
| experimental_batch_schnorr_saturated_block | 240.083 |
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
| QRS worst p99 below individual Schnorr p99 | pass | qrs_worst=95.910 ms, individual_schnorr=345.599 ms |
| QRS worst p99 below experimental batch Schnorr p99 | pass | qrs_worst=95.910 ms, experimental_batch=240.083 ms |

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
