# QRS Reviewer Dossier

This package is Draft-stage pre-review material. It does not claim consensus
readiness.

## What this proposal does

Defines one P2MR-only future leaf version that verifies one
SLH-DSA-SHA2-128s signature over a BIP-341-style transaction digest. See
`docs/bip-p2mr-quantum-rescue-leaf-v0.9.0.mediawiki`.

## What this proposal explicitly does not do

It does not freeze, burn, throttle, sunset, rescue, or otherwise regulate
legacy outputs. It does not add ML-DSA, an algorithm menu, a new address family,
or a tapscript opcode.

## Why P2MR-only

P2MR is key-path-free. A post-quantum rescue leaf inside an output with an
exposed key path would not remove the discrete-log exposure. See the BIP
Rationale section and `01_bip_spec_hardening.md`.

## Why a leaf version instead of opcode

SLH-DSA-SHA2-128s signatures are 7,856 bytes, exceeding BIP-342's retained
520-byte tapscript stack-element limit. A future leaf version keeps this first
primitive narrow. See the BIP Rationale and `test_vectors/`.

## Why SLH-DSA-SHA2-128s

It is stateless and hash-based, keeping the first post-quantum consensus path
on assumptions Bitcoin already relies on through SHA-256. The cost is a large
signature, which receives no witness discount.

## Why no legacy-output policy

No legacy-output policy is proposed here.

Legacy-output policy is a separate social-consensus question. This package
treats that as an explicit non-goal, not an omission.

## Why no witness discount

The signature, public key, control block, and annex are counted under existing
SegWit weight rules. See Resource accounting in the v0.9.0 BIP and
`05_block_validation_model.md`.

## Resource-accounting status

No-new-budget is a hypothesis to test, not an established consensus conclusion.
The harness emits QRS, individual Schnorr, experimental batch Schnorr, and
reviewed-public batch status separately. See `out/full.md` and `out/full.json`.

## Batch Schnorr status

Reviewed public libsecp256k1 batch API: unavailable in the bundled upstream
commit. Experimental native BIP-340 batch baseline: available as sensitivity
analysis only, not reviewed consensus code. See `04_batch_schnorr_baseline.md`
and `scripts/check_batch_schnorr_baseline.py`.

## BIP-360 dependency status

The BIP requires BIP-360 and remains blocked on final BIP-360 definitions for
P2MR hashing, sighash tagging, annex handling, and future-leaf behavior. The
vectors are marked provisional for the same reason.

## Test-vector status

Structured provisional vectors exist in `test_vectors/` and are checked by
`scripts/validate_test_vectors.py`. They distinguish structural rejects from
the invalid fixed-length signature case that reaches SLH-DSA verification.

## Benchmark status

Native OpenSSL SLH-DSA-SHA2-128s, native individual libsecp256k1 BIP-340, and
experimental native batch BIP-340 are measured. The QRS validation-path section
is a model, not Bitcoin Core integration.

## Known blockers before advancing beyond Draft

- Final BIP-360 dependency review.
- Final QRS leaf-version and `ext_flag` conflict review.
- Native benchmark replication across hardware, OS, compilers, and verifier libraries.
- Bitcoin Core validation-path integration, not just the current model.
- Consensus test vectors computed from final BIP-360/QRS definitions.
- Independent SLH-DSA verifier review and cross-implementation vector checks.
