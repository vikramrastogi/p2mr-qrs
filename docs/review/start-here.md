# Review This First

This package is not asking reviewers to support activation.

Primary review artifacts:

- Canonical BIP draft:
  `docs/spec/bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki`
- Resource-accounting decision process:
  `docs/evidence/resource-accounting-decision.md`
- Machine-checked activation and final-vector blockers:
  `docs/evidence/consensus-gap-manifest.md`
- Provisional vector coverage matrix:
  `docs/evidence/vector-coverage-matrix.md`
- Reproduction protocol:
  `docs/reproducibility/reproducibility.md`

## One-sentence proposal

Define one P2MR-only future leaf version that verifies one fixed-size
SLH-DSA-SHA2-128s signature against one fixed-size SLH-DSA-SHA2-128s public key.

## What this BIP does

- Specifies a narrow P2MR SLH-DSA leaf for Draft-stage review.
- Complements BIP-360 by specifying one concrete future-leaf validation rule
  for the P2MR container, rather than replacing BIP-360.
- Uses one 32-byte SLH-DSA-SHA2-128s public key as the leaf payload.
- Uses one 7,856-byte SLH-DSA-SHA2-128s signature in the witness.
- Keeps all QRS witness bytes under existing SegWit weight accounting.
- Provides a native benchmark package, provisional executable fixtures, and
  draft cryptographic vectors for pre-review.

## What this BIP deliberately does not do

- No legacy-output freeze, burn, throttle, sunset, or rescue policy.
- No tapscript opcode in v1.
- No witness discount.
- No algorithm registry.
- No ML-DSA in v1.
- No activation parameters.
- No claim of final consensus-vector coverage.
- No claim of consensus readiness.

## Why not just BIP-360?

BIP-360 defines the P2MR output construction and future-leaf framework. This
package defines one concrete future-leaf validation rule for that framework:
one SLH-DSA-SHA2-128s public key, one fixed-size signature, one digest shape,
one witness shape, and one resource-accounting question. Keeping the leaf
separate lets BIP-360 remain algorithm-neutral and not an algorithm registry.
The dependency is intentional: this draft must be revised to match final
BIP-360 definitions, and it can be folded into BIP-360 if BIP-360 maintainers
prefer that path.

## What would make this proposal invalid

- Final BIP-360/P2MR definitions conflict with the proposed leaf or digest
  structure.
- Reviewers reject a future P2MR leaf version as the right abstraction.
- Native and Core-shaped validation evidence shows weight-only accounting does
  not adequately bound QRS validation cost and the BIP does not add an explicit
  per-QRS validation budget.
- A reviewed verifier strategy cannot provide clear, reproducible
  SLH-DSA-SHA2-128s semantics for the empty-context pure-mode signature.
- Final serialized vectors cannot be made to agree across implementations.

## Known activation blockers

- Final BIP-360 leaf hashing, branch hashing, annex, and future-leaf behavior.
- Final QRS `ext_flag` assignment and SigMsg definition.
- Bitcoin Core consensus validation-path integration.
- Cross-hardware benchmark reproduction.
- Second reviewed SLH-DSA verifier strategy.
- Reviewed-public batch Schnorr baseline or explicit reviewer agreement on how
  to handle its absence.
- Wallet descriptor, PSBT, hardware-signing, and backup standards.
- Activation parameters.
- Final serialized valid and invalid transactions.

## Resource-accounting question

The proposal asks whether the no-additional-budget rule is plausible enough for
Draft review. It does not claim that rule is activation-ready.

Review should focus on whether the benchmark package is the right evidence
shape, whether batch-Schnorr sensitivity is framed correctly, and what exact
Bitcoin Core validation-path artifact would make the resource-accounting
question reviewable.

## Vector status

Current vectors are provisional executable structured fixtures and draft
cryptographic vectors. They are not final consensus vectors. Final serialized
vectors remain blocked on final BIP-360/QRS hashing and sighash definitions.

## Batch Schnorr status

The bundled libsecp256k1 commit does not expose a reviewed public BIP-340 batch
verification API in this package. Experimental batch timing is included only as
sensitivity analysis and must not be cited as reviewed consensus evidence.

## Bitcoin Core integration status

The QRS validation-path benchmark is a local model, not Bitcoin Core consensus
integration. The required Core artifact is described in
`docs/evidence/bitcoin-core-integration-requirements.md`.

## How to reproduce

Run the publication gate on a machine with OpenSSL 3.5 or newer exposing
`SLH-DSA-SHA2-128s`:

```sh
bash scripts/release_check.sh
```

For stricter standard-run evidence:

```sh
bash scripts/full_release_check.sh
```

Linux/macOS CI smoke checks are useful structural evidence, but strict release
evidence still requires the OpenSSL SLH-DSA provider and the local publication
gate.

## Requested reviewer feedback

- Is a future P2MR leaf version the right abstraction?
- Should this remain a dependent BIP, or should BIP-360 absorb it as the
  concrete SLH-DSA future-leaf specification?
- Is the witness shape acceptable?
- Is pure SLH-DSA-SHA2-128s with explicit empty context specified clearly?
- Is `ext_flag = 2` the right provisional digest-separation approach?
- Is weight-only accounting plausible for Draft review?
- What exact Core validation-path artifact would be persuasive?
- Is the explicit QRS budget fallback framed correctly?
