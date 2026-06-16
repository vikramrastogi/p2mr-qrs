# QRS Reviewer Dossier

This package is Draft-stage pre-review material. It does not claim consensus
readiness.

## What this proposal does

Defines one P2MR-only future leaf version that verifies one
SLH-DSA-SHA2-128s signature over a BIP-341-style transaction digest. See
`bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki`.

## What this proposal does not do

It does not freeze, burn, throttle, sunset, rescue, or otherwise regulate
legacy outputs. It does not add ML-DSA, an algorithm menu, a new address family,
or a tapscript opcode.

## Why P2MR-only

P2MR is key-path-free. An SLH-DSA leaf inside an output with an
exposed key path would not remove the discrete-log exposure. See the BIP
Rationale section and `../docs/bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki`.

## Why a leaf version instead of opcode

SLH-DSA-SHA2-128s signatures are 7,856 bytes, exceeding BIP-342's retained
520-byte tapscript stack-element limit. A future leaf version keeps this first
primitive narrow. See the BIP Rationale and `test_vectors/`.

## Why SLH-DSA-SHA2-128s

It is stateless and hash-based, avoids introducing a lattice assumption into
the first post-quantum consensus verifier, and targets NIST Category 1, the
closest NIST post-quantum category to Bitcoin's existing roughly 128-bit
classical security target. The cost is a large signature, which receives no
witness discount.

## Why no legacy-output policy

No legacy-output policy is proposed here.

Legacy-output policy is a separate social-consensus question. This package
treats that as an explicit non-goal, not an omission.

## Why no witness discount

The signature, public key, control block, and annex are counted under existing
SegWit weight rules. See Resource accounting in the v0.9.0 BIP and
`../docs/RESOURCE_ACCOUNTING_DECISION.md`.

## Resource-accounting status

No-new-budget is a hypothesis to test, not an established consensus conclusion.
The harness emits QRS, individual Schnorr, experimental batch Schnorr, and
reviewed-public batch status separately. See `../out/full.md` and
`../out/full.json`.

The generated decision record also includes batch-speedup sensitivity rows. They
are explicitly hypothetical and must not be cited as measured batch evidence;
their purpose is to show where reviewer concern about a faster batch baseline
would make the no-additional-budget rule unresolved.

## Batch Schnorr status

Reviewed public libsecp256k1 batch API: unavailable in the bundled upstream
commit. Experimental native BIP-340 batch baseline: available as sensitivity
analysis only, not reviewed consensus code. See `04_batch_schnorr_baseline.md`
and `../scripts/check_batch_schnorr_baseline.py`.

The experimental batch path includes a BIP-340 challenge self-test. The test
compares libsecp256k1's tagged-SHA helper against an independent
`tagged_hash("BIP0340/challenge", R || P || m)` construction before timing
experimental batch verification.

## BIP-360 dependency status

The BIP requires BIP-360 and remains blocked on final BIP-360 definitions for
P2MR hashing, sighash tagging, annex handling, and future-leaf behavior. The
vectors are marked provisional for the same reason.

## Vector status

Provisional executable structured fixtures exist in `test_vectors/` and are
checked by `scripts/validate_test_vectors.py` and
`scripts/verify_qrs_fixtures.py`. They recompute the modeled hashes and
distinguish structural rejects from the invalid fixed-length signature case
that reaches the modeled SLH-DSA verifier boundary. The model now uses the
current BIP's structural preimage shape where possible: length-prefixed TapLeaf
payloads, TapBranch paths, and a TapSighash message over a SigMsg-shaped
ext_flag=2 fixture payload plus `leaf_hash`.

Draft cryptographic vectors are checked by `scripts/verify_qrs_vectors.py`,
which calls the native SLH-DSA-SHA2-128s verifier for valid and invalid
fixed-length signatures. They are still not final consensus vectors because the
modeled hashing rules remain provisional pending final BIP-360/QRS definitions.

## Benchmark status

Native OpenSSL SLH-DSA-SHA2-128s, native individual libsecp256k1 BIP-340, and
experimental native batch BIP-340 are measured. The QRS validation-path section
is a model, not Bitcoin Core integration. The artifact required to close that
caveat is specified in `BITCOIN_CORE_INTEGRATION_REQUIREMENTS.md`.

## Resource-accounting decision status

`RESOURCE_ACCOUNTING_DECISION.md` records the current decision criteria and
Draft-stage conclusion. It does not establish activation readiness; it states
whether the current native evidence supports continuing to evaluate the
no-additional-budget rule or whether an explicit per-QRS validation budget must
be added before activation.

`scripts/evaluate_resource_accounting.py` turns generated benchmark reports
into `out/resource-accounting-decision.json` and
`out/resource-accounting-decision.md`. If the current evidence fails the stated
criteria, the script reports that an explicit QRS budget is required or that
activation remains unresolved.

The inactive fallback budget design is in
`EXPLICIT_QRS_BUDGET_FALLBACK.md`. It uses additional per-QRS virtual
validation weight rather than a witness discount.

## Reproducibility status

`REPRODUCIBILITY.md` defines the independent rerun protocol and the repository
includes a benchmark-reproduction issue template. Local sample reports are not
independent evidence until another reviewer attaches comparable raw JSON and
Markdown artifacts.

## Objection map

| Objection | Answer | Evidence |
| --- | --- | --- |
| Why not solve old coins? | Legacy-output policy is out of scope. | BIP Abstract and Backward compatibility; this dossier's non-goals. |
| Why not ML-DSA? | This draft tests one conservative hash-based primitive first. | BIP Motivation and Rationale. |
| Why not a tapscript opcode? | The 7,856-byte signature exceeds the retained 520-byte tapscript stack-element limit. | BIP Rationale; `../test_vectors/`. |
| Why no witness discount? | The proposal does not hide a blockspace-policy change inside the crypto primitive. | BIP Resource accounting; `../out/full.md`. |
| What about batch Schnorr? | Reviewed public batch API status is explicit; experimental batch is labeled non-consensus. | `../out/batch-evidence.md`; `../04_batch_schnorr_baseline.md`; `../scripts/check_batch_schnorr_baseline.py`. |
| How does evidence map to the budget rule? | The decision criteria and current conclusion are in a separate decision record. | `RESOURCE_ACCOUNTING_DECISION.md`; `../out/quick.md`; `../out/full.md`. |
| What if the no-budget rule fails? | Use the inactive explicit QRS budget fallback before activation. | `EXPLICIT_QRS_BUDGET_FALLBACK.md`; `../scripts/evaluate_resource_accounting.py`. |
| What if BIP-360 changes? | Advancement is blocked on final BIP-360 definitions. | BIP Dependencies and Rationale; provisional vector notes. |
| Is this activation-ready? | No. It is Draft-stage pre-review material. | `RELEASE_CHECKLIST.md`; this dossier's known blockers. |

## Known blockers before advancing beyond Draft

- Final BIP-360 dependency review.
- Final SLH-DSA leaf-version and `ext_flag` conflict review.
- Native benchmark replication across hardware, OS, compilers, and verifier libraries.
- Bitcoin Core validation-path integration, not just the current model.
- Consensus test vectors with real signatures computed from final
  BIP-360/QRS definitions.
- Independent SLH-DSA verifier review and cross-implementation vector checks.
