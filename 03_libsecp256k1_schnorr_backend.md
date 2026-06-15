# libsecp256k1 Schnorr Backend

Exact goal:
- Implement native BIP-340 Schnorr verification benchmark using libsecp256k1.

Files created:
- `src/schnorr_secp256k1.cpp`
- `src/experimental_batch_schnorr.c`
- `include/qrs_bench/schnorr_secp256k1.h`
- `include/qrs_bench/experimental_batch_schnorr.h`

Current status:
- The backend uses the bundled upstream libsecp256k1 checkout at commit `aea86bc3509f3ceb5c047fe6567bf8910f72a728`.
- The CMake build enables `extrakeys` and `schnorrsig`, disables unrelated modules/tests/benchmarks, and links the harness against the libsecp256k1 target.
- The harness generates a deterministic keypair and valid BIP-340 signature outside timed loops, asserts valid verification, flips one bit to create an invalid fixed-length signature, asserts invalid verification fails, and then times valid and invalid individual verification separately.
- The harness also includes an experimental native BIP-340 batch verifier built from libsecp256k1 internals. It is labeled non-consensus and not a reviewed public libsecp256k1 API.

Required behavior:
- Build against a pinned libsecp256k1 commit or submodule.
- Enable the `schnorrsig` module.
- Generate test keypair and valid BIP-340 signature.
- Verify the valid signature before timing.
- Create invalid fixed-length Schnorr signatures.
- Verify invalid signatures fail before timing.
- Benchmark individual valid verification and invalid verification.
- Record libsecp256k1 commit and compile flags.
- Benchmark experimental batch verification separately from reviewed public batch API status.
- Keep reviewed public batch API status unavailable unless upstream exposes a reviewed public API.

Implementation constraints:
- Do not include signing/keygen in verification timing.
- Reuse context where appropriate, matching realistic validation setup.
- Consume results to prevent dead-code elimination.
- Do not synthesize batch timing from an assumed speedup.

Acceptance commands:

```sh
./build/qrs_native_bench --quick --only schnorr --json out/schnorr.json --markdown out/schnorr.md
jq '.benchmarks.schnorr_bip340.individual_valid_verify.median_ns' out/schnorr.json
```
