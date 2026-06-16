# QRS v0.9.0 Release Checklist

Run:

```sh
bash scripts/release_check.sh
```

Checklist enforced by the script:

- BIP version is v0.9.0.
- BIP includes Requires: 341, 342, 360.
- BIP does not claim activation readiness.
- BIP does not claim a QRS cost advantage over Schnorr.
- Resource accounting says no-new-budget is a hypothesis to test.
- Native QRS benchmark runs.
- Native individual Schnorr benchmark runs.
- Batch Schnorr status is explicit.
- Release checks configure, build, regenerate `out/quick.json` and
  `out/quick.md`, and then validate those freshly generated artifacts.
- Reports include machine/library/compiler metadata.
- Reports include git commit, working-tree dirty status, and benchmark binary
  build mode.
- Release checks fail if a freshly generated report does not match current
  `HEAD`, is generated from a dirty source tree, or reports a libsecp256k1
  commit different from `third_party/secp256k1.COMMIT`.
- Timing stats are labeled as per-batch means.
- Invalid fixed-length signatures are measured across multiple fixed-length
  mutations.
- Structured fixture vectors are model-verified by
  `scripts/verify_qrs_fixtures.py`, and verifier-reaching vectors are
  cryptographically checked by `scripts/verify_qrs_vectors.py`.
- `scripts/check_batch_schnorr_baseline.py` regenerates batch-baseline evidence.
- The experimental batch benchmark reports a BIP-340 challenge self-test that
  compares the libsecp256k1 tagged-SHA helper against an independent
  `tagged_hash("BIP0340/challenge", R || P || m)` construction.
- `scripts/evaluate_resource_accounting.py` regenerates the pass/fail
  resource-accounting decision from the benchmark report.
- `docs/RESOURCE_ACCOUNTING_DECISION.md` maps the benchmark evidence to the
  draft resource-accounting decision.
- `docs/EXPLICIT_QRS_BUDGET_FALLBACK.md` specifies the inactive per-QRS budget
  fallback if weight-only accounting fails.
- `docs/BITCOIN_CORE_INTEGRATION_REQUIREMENTS.md` states the exact Core
  validation-path artifact required before activation.
- `docs/FINAL_TRANSACTION_VECTOR_SCHEMA.md` and
  `test_vectors/qrs_transaction_vector.schema.json` define the final serialized
  vector shape once BIP-360/QRS definitions are final.
- `docs/REPRODUCIBILITY.md` and the benchmark reproduction issue template
  define the independent rerun protocol.
- BIP-360 final dependency is documented.
- README states package is Draft-stage pre-review.

Manual release blockers:

- Do not claim all caveats are nullified: Core integration, final BIP-360/QRS
  transaction vectors, reviewed public batch Schnorr, second SLH-DSA
  implementation, and independent reproduction are external activation
  blockers until their artifacts exist.
- Replace provisional structured vectors with final transaction vectors once
  BIP-360 and QRS sighash definitions are final.
- Treat experimental batch Schnorr only as sensitivity analysis.
- Do not post as activation-ready.
