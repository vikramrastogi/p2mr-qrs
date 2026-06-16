# Bitcoin Core Validation-Path Integration Requirements

Status: Bitcoin Core validation-path integration not implemented in this
repository.

The native benchmark harness models the QRS validation buckets. It does not
replace a Bitcoin Core validation-path benchmark. This document defines the
minimum artifact needed to close that caveat.

## Required Patch Surface

A Core-facing prototype must exercise QRS spends through the block/script
validation path, not through a standalone verifier loop. At minimum it must
include:

- P2MR/QRS witness parsing at the future-leaf boundary.
- QRS structural checks: witness shape, signature length, public-key length,
  control-block length, low-bit rule, leaf-version rule, and Merkle-root check.
- QRS signature-message construction using the final BIP-360/QRS digest rules.
- FIPS 205 SLH-DSA-SHA2-128s verification in pure mode with an empty context.
- Consensus tests for valid QRS, invalid fixed-length QRS, structural rejects,
  annex handling, and maximum-depth control paths.
- Block-validation benchmarks using QRS-heavy blocks and comparable
  Schnorr-heavy blocks.

## Required Measurements

The Core benchmark report must include:

- valid QRS block validation time;
- worst observed invalid fixed-length QRS block validation time;
- structural-reject timing separated from verifier-reaching invalid timing;
- individual Schnorr baseline timing through the same validation path;
- reviewed public batch Schnorr baseline if available;
- raw block weight, QRS spend count, Schnorr spend count, and transaction
  shape;
- hardware, OS, compiler, compiler flags, Core commit, SLH-DSA implementation
  commit, libsecp256k1 commit, and benchmark harness commit.

## Passing Condition

The modeled harness is sufficient for pre-review only. Before activation, the
Core report must feed `scripts/evaluate_resource_accounting.py` or an equivalent
reviewed evaluator and produce one of:

- `supports_no_additional_budget_for_draft`, with the remaining activation
  blockers independently resolved; or
- `unresolved_requires_reviewer_decision`, with the batch-baseline or policy
  question explicitly decided before activation; or
- `explicit_budget_required_before_activation`, followed by adoption of
  `docs/EXPLICIT_QRS_BUDGET_FALLBACK.md` with a positive reviewed constant.

## Non-Claim

Until this artifact exists, the repository must continue to say that QRS
validation-path evidence is modeled and not Bitcoin Core consensus integration.
