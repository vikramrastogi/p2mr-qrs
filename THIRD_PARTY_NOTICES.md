# Third-Party Notices

This repository vendors upstream code only where the benchmark harness needs a
pinned dependency for reproducible review.

## bitcoin-core/secp256k1

- Path: `third_party/secp256k1/`
- Pinned commit: see `third_party/secp256k1.COMMIT`
- Upstream: <https://github.com/bitcoin-core/secp256k1>
- License: MIT-style license in `third_party/secp256k1/COPYING`

The QRS benchmark code in this repository is separate from upstream
libsecp256k1. The experimental batch Schnorr code is local benchmark code for
sensitivity analysis only; it is not a reviewed public libsecp256k1 API and is
not consensus-grade validation code.
