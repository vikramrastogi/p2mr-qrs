# OpenSSL SLH-DSA Backend

Exact goal:
- Implement native FIPS 205 SLH-DSA-SHA2-128s verification benchmark using OpenSSL.

Files created:
- `src/slh_dsa_openssl.cpp`
- `include/qrs_bench/slh_dsa_openssl.h`

Required behavior:
- Use OpenSSL EVP APIs for `SLH-DSA-SHA2-128s`.
- Generate a test keypair.
- Sign a 32-byte QRS-style message in pure mode with empty context.
- Export the raw public key, assert it is exactly 32 bytes, import that public key, and use the imported public key for verification timing.
- Assert the signature is exactly 7,856 bytes.
- Verify the valid signature before timing.
- Create invalid fixed-length signatures by bit-flipping deterministic positions.
- Verify that invalid fixed-length signatures fail before timing.
- Benchmark valid verification and invalid fixed-length verification separately.
- Record OpenSSL version and provider information.

Implementation constraints:
- Use pure SLH-DSA, not pre-hash mode.
- Set the SLH-DSA context string parameter explicitly to empty through OpenSSL parameters.
- Accept only 32-byte public key and 7,856-byte signature for the QRS parameter set.
- Do not time key generation or signing in the verification benchmark.

Acceptance commands:

```sh
./build/qrs_native_bench --quick --only slh-dsa --json out/slh.json --markdown out/slh.md
jq '.benchmarks.slh_dsa_sha2_128s.valid_verify.median_ns' out/slh.json
jq '.benchmarks.slh_dsa_sha2_128s.invalid_fixed_length_verify.median_ns' out/slh.json
```
