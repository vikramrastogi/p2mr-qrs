# Resource Accounting Evaluation

## Pass/Fail Conclusion

- `draft_rule_status`: activation_unresolved_batch_baseline
- `activation_ready`: false
- `explicit_qrs_budget_required`: false
- `fallback_budget_status`: reviewer_decision_required
- `conclusion`: QRS is below individual Schnorr but exceeds experimental batch Schnorr; resource accounting remains unresolved for activation.

## Compared Estimates

| Estimate | p99 ms |
| --- | ---: |
| qrs_valid_saturated_block | 92.037 |
| qrs_worst_invalid_fixed_length_saturated_block | 256.198 |
| individual_schnorr_saturated_block | 399.126 |
| experimental_batch_schnorr_saturated_block | 242.702 |
| reviewed_public_batch_schnorr_saturated_block | unavailable |

## Checks

| Check | Result | Detail |
| --- | --- | --- |
| SLH-DSA timings available | pass | available |
| QRS validation-path model available | pass | available |
| Individual Schnorr baseline available | pass | available |
| Reviewed public batch status explicit | pass | unavailable |
| Batch evidence forbids fake speedups | pass | No batch Schnorr timing is synthesized from assumed speedups. |
| QRS worst p99 below individual Schnorr p99 | pass | qrs_worst=256.198 ms, individual_schnorr=399.126 ms |
| QRS worst p99 below experimental batch Schnorr p99 | fail | qrs_worst=256.198 ms, experimental_batch=242.702 ms |

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
