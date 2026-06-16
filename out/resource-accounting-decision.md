# Resource Accounting Evaluation

## Pass/Fail Conclusion

- `draft_rule_status`: pass_draft_no_additional_budget_supported
- `activation_ready`: false
- `explicit_qrs_budget_required`: false
- `fallback_budget_status`: not_required_by_current_draft_evidence
- `conclusion`: Current native evidence supports continuing with the no-additional-budget rule as Draft-stage review material.

## Compared Estimates

| Estimate | p99 ms |
| --- | ---: |
| qrs_valid_saturated_block | 100.561 |
| qrs_worst_invalid_fixed_length_saturated_block | 102.120 |
| individual_schnorr_saturated_block | 358.329 |
| experimental_batch_schnorr_saturated_block | 239.453 |
| reviewed_public_batch_schnorr_saturated_block | unavailable |

## Checks

| Check | Result | Detail |
| --- | --- | --- |
| SLH-DSA timings available | pass | available |
| QRS validation-path model available | pass | available |
| Individual Schnorr baseline available | pass | available |
| Reviewed public batch status explicit | pass | unavailable |
| Batch evidence forbids fake speedups | pass | No batch Schnorr timing is synthesized from assumed speedups. |
| QRS worst p99 below individual Schnorr p99 | pass | qrs_worst=102.120 ms, individual_schnorr=358.329 ms |
| QRS worst p99 below experimental batch Schnorr p99 | pass | qrs_worst=102.120 ms, experimental_batch=239.453 ms |

## Activation Blockers

- Reviewed public batch Schnorr baseline unavailable.
- Bitcoin Core validation-path integration not implemented.
- Final BIP-360/QRS transaction digest definitions not available.
- Independent cross-hardware reproduction not attached.
- Second reviewed SLH-DSA implementation not wired.

This generated file is release evidence, not consensus activation evidence.
Activation still requires final BIP-360/QRS definitions, Bitcoin Core
validation-path integration, independent reproduction, implementation review,
and reviewer agreement on the batch-Schnorr baseline question.
