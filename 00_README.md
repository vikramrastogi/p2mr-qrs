# P2MR SLH-DSA Leaf Pre-Review Package

Status: Draft-stage pre-review package for v0.9.0. This is not
activation-ready consensus evidence.

This repository contains a proposed BIP draft and native benchmark package for
one P2MR-only SLH-DSA-SHA2-128s leaf. The package is intended to make review
objections concrete: each major caveat is represented by executable code,
benchmark evidence, an explicit non-goal, an explicit blocker before
advancement, or a reproducible release check.

## Preserved Design Scope

- P2MR-only future leaf.
- One SLH-DSA-SHA2-128s signature.
- One 32-byte SLH-DSA-SHA2-128s public key.
- No legacy-output freeze, burn, throttle, sunset, or rescue policy.
- No witness discount.
- No algorithm registry.
- No ML-DSA in v1.
- No tapscript opcode in v1.
- Resource accounting remains Draft-stage and evidence-gated.

## Evidence Package

- `docs/bip-p2mr-quantum-rescue-leaf-v0.9.0.mediawiki` is the BIP draft.
- `docs/REVIEWER_DOSSIER.md` maps predictable objections to evidence,
  non-goals, or blockers.
- `src/` and `include/` implement the native benchmark harness.
- `scripts/evaluate_resource_accounting.py` generates the resource-accounting
  decision from benchmark JSON.
- `scripts/release_check.sh` rebuilds the benchmark binary, regenerates reports,
  verifies vectors, and checks the public release contract.
- `test_vectors/` contains provisional executable structured fixtures and draft
  cryptographic vectors. Final consensus vectors are blocked on final
  BIP-360/QRS hashing and sighash definitions.

## Release Check

```sh
bash scripts/release_check.sh
```

The strict release check requires OpenSSL 3.5 or newer with a loaded provider
that exposes `SLH-DSA-SHA2-128s` through EVP. The batch-Schnorr upstream scan is
local-only by default; set `QRS_RELEASE_NETWORK_BATCH=1` when publishing
network-checked upstream evidence.

## Current Proof Status

Native QRS, individual Schnorr, and experimental batch Schnorr timings can be
generated from the current source tree. The experimental batch baseline is
sensitivity analysis only. Reviewed-public batch Schnorr remains a distinct
unavailable baseline unless upstream provides a reviewed public API or reviewed
experimental branch.
