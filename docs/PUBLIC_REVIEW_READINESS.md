# Public Review Readiness

This repository is ready for public pre-review only if:

- `bash scripts/release_check.sh` passes on a machine with OpenSSL 3.5+ exposing
  SLH-DSA-SHA2-128s.
- The canonical BIP file is `docs/bip-p2mr-slh-dsa-leaf-v0.9.0.mediawiki`.
- The BIP states Draft status and does not claim activation readiness.
- Provisional vectors are not described as final consensus vectors.
- Resource-accounting output says `activation_ready = false`.
- Reviewed-public batch Schnorr remains distinguished from experimental batch.
- Bitcoin Core integration remains listed as a blocker.
- Final BIP-360/QRS definitions remain listed as a blocker.
- No private scaffolding, AI/process artifacts, or unsupported prior-draft acknowledgements remain.
