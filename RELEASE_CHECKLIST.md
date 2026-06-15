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
- Reports include machine/library/compiler metadata.
- Timing stats are labeled as per-batch means.
- Invalid fixed-length signatures are measured.
- Test vectors exist or are marked pending.
- BIP-360 final dependency is documented.
- README states package is Draft-stage pre-review.

Manual release blockers:

- Replace provisional structured vectors with final transaction vectors once
  BIP-360 and QRS sighash definitions are final.
- Treat experimental batch Schnorr only as sensitivity analysis.
- Do not post as activation-ready.
