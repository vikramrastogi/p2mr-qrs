# SLH-DSA Verification Cost Analysis

Status: Draft-stage analysis note.

This package contains empirical timing evidence for OpenSSL
SLH-DSA-SHA2-128s verification and several fixed-length invalid-signature
cases. That evidence is useful, but it is not an algorithmic worst-case proof
and it is not activation readiness.

## Empirical Timing Evidence

The benchmark harness measures:

- valid SLH-DSA-SHA2-128s verification;
- fixed-length invalid signatures that reach the verifier;
- wrong-message and wrong-public-key invalid cases;
- per-batch median, p95, p99, and maximum timing summaries.

The invalid suite is intended to catch obvious fast-reject/slow-reject
asymmetries. random bit flips are not a proof of worst-case verifier cost.

## Algorithmic Worst-Case

Before activation review, QRS needs an algorithmic worst-case analysis for the
selected SLH-DSA verifier. That analysis should identify whether verification
work is input-independent enough for weight-only accounting, including WOTS+
chain evaluation, FORS verification, hypertree traversal, address derivation,
and hash-function call counts.

The current BIP posture is therefore:

- empirical timing evidence can support Draft review;
- algorithmic worst-case review remains required;
- second-backend agreement remains required;
- Bitcoin Core-shaped validation-path timing remains required.

## Reviewer Question

If reviewers find a verifier path whose fixed-length invalid signatures can
force materially higher work than the current suite observes, the no-additional
budget rule should remain unresolved or switch to the explicit QRS budget
fallback before activation.
