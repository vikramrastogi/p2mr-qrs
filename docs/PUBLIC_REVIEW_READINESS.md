# Public Review Readiness

This repository is ready for public pre-review only if:

- `bash scripts/release_check.sh` passes on a machine with OpenSSL 3.5+ exposing
  SLH-DSA-SHA2-128s.
- The canonical BIP file is `docs/bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki`.
- The BIP states Draft status and does not claim activation readiness.
- The public framing describes this as complementary to BIP-360, not as a
  replacement for it.
- Provisional vectors are not described as final consensus vectors.
- Resource-accounting output says `activation_ready = false`.
- Reviewed-public batch Schnorr remains distinguished from experimental batch.
- Bitcoin Core integration remains listed as a blocker.
- Final BIP-360/QRS definitions remain listed as a blocker.
- `docs/CONSENSUS_GAP_MANIFEST.md` remains the machine-checked activation and
  final-vector blocker list.
- No private scaffolding, AI/process artifacts, or unsupported prior-draft acknowledgements remain.

Before posting, read `REVIEW_THIS_FIRST.md` and `PUBLIC_REVIEW_POST_DRAFT.md`
and confirm they still match the current `BIP360_DEPENDENCY_MATRIX.md`,
resource-accounting decision output, and vector status.

The highest-leverage public path is coordination with BIP-360 review first:
present this package as a concrete SLH-DSA future-leaf candidate for P2MR, not
as a competing P2MR proposal.
