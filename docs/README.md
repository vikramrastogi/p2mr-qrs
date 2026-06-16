# Documentation Map

Start with `review/start-here.md`.

## Review Funnel

- `review/start-here.md` - reviewer entrypoint and requested feedback.
- `review/reviewer-dossier.md` - predictable objections mapped to evidence,
  blockers, or non-goals.
- `review/public-review-readiness.md` - public pre-review readiness checklist.
- `review/public-review-post-draft.md` - short public-posting draft.

## Specification

- `spec/bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki` - canonical BIP draft.
- `spec/bip360-dependency-matrix.md` - final BIP-360/P2MR dependency table.
- `spec/final-transaction-vector-schema.md` - final serialized vector contract.
- `spec/core-validation-model.md` - modeled validation path.

## Evidence

- `evidence/resource-accounting-decision.md` - decision criteria and current
  Draft-stage resource-accounting conclusion.
- `evidence/explicit-qrs-budget-fallback.md` - inactive fallback if weight-only
  accounting fails review.
- `evidence/batch-schnorr-baseline-status.md` - reviewed-public and
  experimental batch Schnorr status.
- `evidence/bitcoin-core-integration-requirements.md` - Core artifact required
  to close the validation-path caveat.
- `evidence/slh-dsa-verify-cost-analysis.md` - verifier-cost evidence and
  algorithmic worst-case caveat.
- `evidence/vector-coverage-matrix.md` - provisional fixture and negative-case
  coverage.
- `evidence/consensus-gap-manifest.md` - machine-checked activation and
  final-vector blocker list.

## Reproducibility

- `reproducibility/reproducibility.md` - independent rerun protocol.
- `reproducibility/reproduction-matrix.md` - platform reproduction targets.
- `reproducibility/release-checklist.md` - release-gate checklist.
- `reproducibility/native-benchmark-report-template.md` - report template.

## Internal Notes

- `internal/pre-review-package.md` - package overview used by release checks.
- `internal/move-map.md` - old-to-new path map for this documentation reorg.
- `internal/implementation-notes/` - implementation notes retained for
  maintainers.

## Top-Level Artifact Directories

- `src/` and `include/` - native benchmark harness.
- `scripts/` - validation, report, and evidence scripts.
- `test_vectors/` - provisional executable fixtures and draft cryptographic
  vectors.
- `fuzz/` and `tests/` - structured malformed-witness corpus and negative cases.
- `out/` - committed sample artifacts. These remain sample outputs and are not
  reinterpreted by the documentation reorg.
- `third_party/` - vendored pinned libsecp256k1 source.
