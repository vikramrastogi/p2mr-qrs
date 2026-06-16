# QRS Native Benchmark Report

Mode: `standard`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: a76bb1c988678130fadfa041f68d1e5355e2491e
- `benchmark_schema`: qrs-native-bench/v0
- `build_type`: Release/NDEBUG
- `cmake_build_type`: Release
- `compiler`: clang
- `compiler_flags`: -O3 -Wall -Wextra -Wpedantic
- `compiler_version`: 17.0.0 (clang-1700.6.4.2)
- `cpu_features`: hw.optional.arm.FEAT_AES=1, hw.optional.arm.FEAT_PMULL=1, hw.optional.arm.FEAT_SHA1=1, hw.optional.arm.FEAT_SHA256=1, hw.optional.arm.FEAT_SHA3=1, hw.optional.arm.FEAT_SHA512=1, hw.optional.arm.FEAT_LSE=1, hw.optional.arm.FEAT_FP16=1
- `cpu_frequency`: unavailable
- `cpu_model`: Apple M2 Pro
- `frequency_scaling`: not controlled by harness; reviewer must record governor/power mode
- `git_commit`: a76bb1c988678130fadfa041f68d1e5355e2491e
- `openssl_provider`: default
- `openssl_slh_dsa_algorithm`: SLH-DSA-SHA2-128s
- `openssl_slh_dsa_mode`: EVP direct message signing/verification, no prehash digest, context-string explicitly set to empty
- `openssl_version`: OpenSSL 3.6.1 27 Jan 2026
- `os`: Darwin 25.3.0 arm64
- `working_tree_dirty`: true
- `OpenSSL`: OpenSSL 3.6.1 27 Jan 2026
- `OpenSSL provider`: default
- `SLH-DSA mode`: EVP direct message signing/verification, no prehash digest, context-string explicitly set to empty
- `SLH-DSA public key bytes`: 32
- `SLH-DSA signature bytes`: 7856
- `libsecp256k1`: aea86bc3509f3ceb5c047fe6567bf8910f72a728

## SLH-DSA Backend Status

| Backend | Status | Provider | Version | Public key bytes | Signature bytes | Reason |
|---|---:|---|---|---:|---:|---|
| openssl | available | default | OpenSSL 3.6.1 27 Jan 2026 | 32 | 7856 |  |
| second_reviewed_backend | unavailable | unavailable | unavailable | 32 | 7856 | second reviewed SLH-DSA backend not wired |

The second reviewed SLH-DSA backend is reported separately from the OpenSSL backend; it remains unavailable until a reviewed/pinned verifier strategy is wired for cross-backend agreement.

## SLH-DSA Backend Probe

| Probe | Result |
|---|---|
| keygen | passed |
| sign | passed |
| verify valid | passed |
| verify mutated signature | passed |
| public key bytes | 32 |
| signature bytes | 7856 |
| context string | explicit_empty_context_set |

## Raw Timing Table

Timing statistics are computed as the median of per-batch means, p95 of per-batch means, p99 of per-batch means, and max of per-batch means; they are not individual operation latency samples.

| Benchmark | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| SLH-DSA-SHA2-128s valid verify | available | 183848.969 | 188660.297 | 188745.609 | 188766.938 | 5428.887 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 198654.938 | 206067.438 | 208849.938 | 209545.562 | 5024.580 |
| BIP-340 Schnorr individual valid verify | available | 20571.922 | 20940.562 | 21114.107 | 21165.438 | 48637.842 |
| BIP-340 Schnorr individual invalid verify | available | 20468.594 | 21030.150 | 22329.687 | 22726.677 | 48708.737 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13639.450 | 13757.431 | 13768.903 | 13771.771 | 73299.992 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 192453.125 | 195391.528 | 195580.406 | 195627.625 | 5177.081 |
| middle_bitflip | available | 183526.031 | 189709.263 | 190539.777 | 190747.406 | 5426.299 |
| last_bitflip | available | 186587.250 | 190542.575 | 193232.465 | 193904.938 | 5375.494 |
| spread_32_bitflips | available | 177458.344 | 184231.647 | 184554.679 | 184635.438 | 5602.230 |
| all_zero | available | 190113.281 | 197688.281 | 197937.656 | 198000.000 | 5233.240 |
| all_ff | available | 198654.938 | 206067.438 | 208849.938 | 209545.562 | 5024.580 |
| deterministic_garbage | available | 195436.188 | 204755.075 | 206696.840 | 207182.281 | 5076.608 |
| wrong_message | available | 187957.031 | 193406.916 | 195043.883 | 195453.125 | 5301.495 |
| wrong_public_key | available | 188503.906 | 195096.091 | 195399.418 | 195475.250 | 5302.742 |
| min observed invalid fixed-length | available | 177458.344 | 184231.647 | 184554.679 | 184635.438 | 5602.230 |
| best observed invalid fixed-length | available | 177458.344 | 184231.647 | 184554.679 | 184635.438 | 5602.230 |
| median observed invalid fixed-length | available | 188503.906 | 195096.091 | 195399.418 | 195475.250 | 5302.742 |
| p99 observed invalid fixed-length | available | 198654.938 | 206067.438 | 208849.938 | 209545.562 | 5024.580 |
| worst observed invalid fixed-length | available | 198654.938 | 206067.438 | 208849.938 | 209545.562 | 5024.580 |

Worst observed invalid fixed-length case: `all_ff`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields using explicit non-consensus prefixes QRS modeled TapLeaf v0, QRS modeled Branch v0, and QRS modeled SigMsg v0; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 352.625 | 364.290 | 364.346 | 364.359 | 2810883.215 |
| qrs_msg construction | available | 729.974 | 749.880 | 755.305 | 756.661 | 1373907.509 |
| verifier valid call | available | 173199.219 | 177034.628 | 178275.676 | 178585.938 | 5792.047 |
| total modeled valid path | available | 175000.000 | 177198.828 | 178867.891 | 179285.156 | 5742.079 |
| invalid structural reject | available | 2.651 | 3.068 | 3.068 | 3.068 | 365158965.870 |
| invalid fixed-length crypto reject | available | 180643.219 | 186930.609 | 187046.547 | 187075.531 | 5508.562 |

## Batch Schnorr Baseline

- `reviewed_public_api_status`: unavailable
- `reviewed_public_api_reason`: Bundled upstream bitcoin-core/secp256k1 commit aea86bc3509f3ceb5c047fe6567bf8910f72a728 exposes only the public secp256k1_schnorrsig_verify API for BIP-340; its schnorrsig tests still mark verify_batch as TODO. The separate scripts/check_batch_schnorr_baseline.py evidence report records local API status and optional upstream-head lookup status. No fake batch baseline is synthesized.
- `experimental_status`: available
- `experimental_label`: Experimental native BIP-340 batch baseline; not a reviewed public libsecp256k1 API.
- BIP-340 challenge self-test: `available` - BIP-340 challenge self-test passed: libsecp256k1 tagged SHA helper matched an independent tagged_hash construction over deterministic inputs and over the R || P || m tuple from a generated valid signature.
- `coefficient_policy`: Deterministic nonzero coefficients derived from a domain-separated hash of all batch inputs; a_0 = 1.
- `invalid_batch_policy`: Invalid batch rejects as a batch; no fallback to individual verification is timed.
- `timing_scope`: Timed verification includes signature/public-key parsing, BIP-340 challenge computation, coefficient derivation, and the multiscalar batch equation check.
- `policy`: no assumed speedup is applied; without a reviewed public batch Schnorr implementation, the batch baseline remains unavailable.

Reviewed public libsecp256k1 batch API: unavailable.
Experimental native BIP-340 batch baseline: available.
This benchmark implements the experimental native BIP-340 batch baseline for sensitivity analysis only; it is not a reviewed public libsecp256k1 API and must not be treated as consensus-grade validation code.

## Experimental Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1421201.500 | 1449227.033 | 22206.273 | 22644.172 |
| 128 | available | 2487416.667 | 2660393.750 | 19432.943 | 20784.326 |
| 512 | available | 8764013.833 | 9046667.467 | 17117.215 | 17669.272 |
| 2048 | available | 31674361.333 | 32359985.900 | 15465.997 | 15800.774 |
| 17391 | available | 237203667.000 | 239454995.800 | 13639.450 | 13768.903 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1342236.167 | 1391847.817 | 20972.440 | 21747.622 |
| 128 | available | 2515132.000 | 2529898.650 | 19649.469 | 19764.833 |
| 512 | available | 8741173.667 | 9317865.317 | 17072.605 | 18198.956 |
| 2048 | available | 30981875.000 | 31674331.867 | 15127.869 | 15465.982 |
| 17391 | available | 234221833.000 | 245290861.900 | 13467.991 | 14104.471 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs. QRS depth rows include the control-block witness weight for the Merkle path and report the resulting TapBranch hash-operation count; timing estimates still use the measured QRS validation-path timing multiplied by the depth-specific weight-limited input count.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 91.189 | 93.576 | 93.618 | 93.628 |
| QRS invalid fixed-length saturated block | available | 496 | 98.533 | 102.209 | 103.590 | 103.935 |
| Schnorr individual saturated block | available | 17391 | 357.766 | 364.177 | 367.195 | 368.088 |
| Schnorr experimental batch saturated block | available | 17391 | 237.204 | 239.255 | 239.455 | 239.505 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS depth-sensitive saturated block estimates

| Model | QRS Merkle depth | Status | Input WU | Max inputs | TapBranch hashes/block | Median ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | 0 | available | 8059 | 496 | 0 | 91.189 | 93.618 | 93.628 |
| QRS valid saturated block | 1 | available | 8091 | 494 | 494 | 90.821 | 93.240 | 93.251 |
| QRS valid saturated block | 3 | available | 8155 | 490 | 1470 | 90.086 | 92.485 | 92.496 |
| QRS valid saturated block | 128 | available | 12155 | 329 | 42112 | 60.486 | 62.097 | 62.104 |
| QRS invalid fixed-length saturated block | 0 | available | 8059 | 496 | 0 | 98.533 | 103.590 | 103.935 |
| QRS invalid fixed-length saturated block | 1 | available | 8091 | 494 | 494 | 98.136 | 103.172 | 103.516 |
| QRS invalid fixed-length saturated block | 3 | available | 8155 | 490 | 1470 | 97.341 | 102.336 | 102.677 |
| QRS invalid fixed-length saturated block | 128 | available | 12155 | 329 | 42112 | 65.357 | 68.712 | 68.940 |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 93.618
- `experimental_batch_schnorr_p99_ms`: 239.455

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
