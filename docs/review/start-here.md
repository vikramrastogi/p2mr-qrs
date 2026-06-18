# Start Here

This package is not asking reviewers to support activation.

It is a Draft-stage pre-review package for one concrete P2MR future leaf:
one SLH-DSA-SHA2-128s public key, one SLH-DSA-SHA2-128s signature, and
SIGHASH_DEFAULT only.

## Primary Artifacts

- Canonical BIP draft:
  `docs/spec/bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki`
- BIP-360 dependency matrix:
  `docs/spec/bip360-dependency-matrix.md`
- Resource-accounting decision:
  `docs/evidence/resource-accounting-decision.md`
- Full sample benchmark report:
  `out/full.md`
- Machine-checked blocker manifest:
  `docs/evidence/consensus-gap-manifest.md`
- Provisional vector coverage matrix:
  `docs/evidence/vector-coverage-matrix.md`
- Reproduction protocol:
  `docs/reproducibility/reproducibility.md`

## Scope

The proposal defines one P2MR-only future leaf version that verifies one
fixed-size SLH-DSA-SHA2-128s signature against one fixed-size
SLH-DSA-SHA2-128s public key.

It deliberately does not include:

- No legacy-output freeze, burn, throttle, sunset, or rescue policy.
- No tapscript opcode in v1.
- No witness discount.
- No algorithm registry.
- No ML-DSA in v1.
- No activation parameters.
- No claim of final consensus-vector coverage.
- No claim of consensus readiness.

## BIP-360 Relationship

BIP-360 defines the P2MR output construction and future-leaf framework. This
package defines one concrete future-leaf validation rule for that framework.
Keeping the leaf separate lets BIP-360 remain algorithm-neutral and not an
algorithm registry.

The dependency is intentional. This draft must be revised to match final
BIP-360 definitions, and it can be folded into BIP-360 if BIP-360 maintainers
prefer that path.

## Resource-Accounting Status

The no-additional-budget rule is a hypothesis to test at Draft stage, not
activation evidence. The current package includes native benchmark machinery,
individual Schnorr timing, experimental batch-Schnorr sensitivity analysis, and
an explicit fallback path if reviewers conclude weight-only accounting is not
adequate.

The experimental batch baseline is not a reviewed public libsecp256k1 API and
must not be cited as consensus evidence.

## Vector Status

Current vectors are provisional executable structured fixtures and draft
cryptographic vectors. They are not final consensus vectors. Final serialized
vectors remain blocked on final BIP-360/QRS hashing and sighash definitions.
Final serialized vectors remain blocked until those definitions are stable.

## Known Blockers Before Advancing Beyond Draft

- Final BIP-360 leaf hashing, branch hashing, annex, and future-leaf behavior.
- Final BIP-360 zero-depth / single-leaf P2MR tree behavior.
- Final QRS `ext_flag` assignment and SigMsg definition.
- Bitcoin Core consensus validation-path integration.
- Cross-hardware benchmark reproduction.
- Second reviewed SLH-DSA verifier strategy.
- Reviewed-public batch Schnorr baseline or explicit reviewer agreement on how
  to handle its absence.
- Wallet descriptor, PSBT, hardware-signing, and backup standards.
- Activation parameters.
- Final serialized valid and invalid transactions.

## Reproduce

Run the publication gate on a machine with OpenSSL 3.5 or newer exposing
`SLH-DSA-SHA2-128s`:

```sh
bash scripts/release_check.sh
```

For stricter standard-run evidence:

```sh
bash scripts/full_release_check.sh
```

## Requested Review

- Is a future P2MR leaf version the right abstraction?
- Should this remain a dependent BIP, or should BIP-360 absorb it?
- Is the witness shape acceptable?
- Is pure SLH-DSA-SHA2-128s with explicit empty context specified clearly?
- Is `ext_flag = 2` the right provisional digest-separation approach?
- Is weight-only accounting plausible for Draft review?
- What exact Core validation-path artifact would be persuasive?
