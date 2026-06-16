# Explicit QRS Validation Budget Fallback

Status: inactive fallback design.

The BIP currently proposes no additional QRS validation budget beyond SegWit
weight. If native review rejects that rule, the activation-ready draft should
switch to this explicit fallback instead of inventing a new mechanism late in
review.

## Rule Shape

Keep ordinary transaction weight unchanged. In addition, define a consensus
virtual validation-weight charge for each QRS spend:

```text
QRS_ADDITIONAL_VALIDATION_WU = reviewed activation constant
effective_block_weight =
    segwit_block_weight
  + qrs_verification_count * QRS_ADDITIONAL_VALIDATION_WU
```

A block is valid only if:

```text
effective_block_weight <= 4,000,000
```

`QRS_ADDITIONAL_VALIDATION_WU = 0` is the current proposed rule. Any positive
value is an explicit per-QRS validation budget.

## What Counts

`qrs_verification_count` counts each structurally valid QRS witness that reaches
the SLH-DSA verifier. Cheap structural rejects are already invalid spends and
do not become consensus-valid merely because they avoid the verifier.

The fallback does not discount QRS witness bytes. The signature, public key,
control block, and annex still contribute their full SegWit weight.

## Constant Selection

The activation constant must be chosen before deployment from native benchmark
review. The review should compare the worst observed QRS p99 saturated-block
estimate against the accepted Schnorr baseline:

```text
qrs_worst_p99_ms =
    max(qrs_valid_saturated_block_p99_ms,
        qrs_invalid_fixed_length_saturated_block_p99_ms)
```

If weight alone is inadequate, a conservative activation candidate is:

```text
QRS_ADDITIONAL_VALIDATION_WU =
    ceil(qrs_input_weight_wu * (qrs_worst_p99_ms / accepted_schnorr_p99_ms))
  - qrs_input_weight_wu
```

rounded up to a simple reviewed constant and never below zero.

The accepted Schnorr baseline must be stated explicitly. It may be individual
BIP-340 verification, a reviewed public batch baseline, or another baseline
reviewers choose. Experimental batch timing in this package is sensitivity
analysis, not a reviewed baseline by itself.

## Release Gate

If `scripts/evaluate_resource_accounting.py` reports
`explicit_qrs_budget_required: true`, the BIP must not advance with
`QRS_ADDITIONAL_VALIDATION_WU = 0`. The draft must either:

- set a positive reviewed `QRS_ADDITIONAL_VALIDATION_WU`; or
- remain Draft-stage and explicitly unresolved.

This fallback is deliberately formulaic so the reviewer debate is about the
accepted baseline and activation constant, not about whether an escape hatch
exists.
