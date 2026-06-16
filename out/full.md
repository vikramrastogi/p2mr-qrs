# QRS Native Benchmark Report

Mode: `full`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: 5eba230f958e28366e52bdcf020bf646e04b16ab
- `build_type`: Release/NDEBUG
- `compiler`: clang 17.0.0 (clang-1700.6.4.2)
- `compiler_flags`: CMake target: -O3 -Wall -Wextra -Wpedantic
- `cpu_features`: hw.optional.arm.FEAT_AES=1, hw.optional.arm.FEAT_PMULL=1, hw.optional.arm.FEAT_SHA1=1, hw.optional.arm.FEAT_SHA256=1, hw.optional.arm.FEAT_SHA3=1, hw.optional.arm.FEAT_SHA512=1, hw.optional.arm.FEAT_LSE=1, hw.optional.arm.FEAT_FP16=1
- `cpu_frequency`: unavailable
- `cpu_model`: Apple M2 Pro
- `frequency_scaling`: not controlled by harness; reviewer must record governor/power mode
- `git_commit`: 5eba230f958e28366e52bdcf020bf646e04b16ab
- `os`: Darwin 25.3.0 arm64
- `working_tree_dirty`: false
- `OpenSSL`: OpenSSL 3.6.1 27 Jan 2026
- `OpenSSL provider`: default
- `SLH-DSA mode`: EVP direct message signing/verification, no prehash digest, context-string explicitly set to empty
- `SLH-DSA public key bytes`: 32
- `SLH-DSA signature bytes`: 7856
- `libsecp256k1`: aea86bc3509f3ceb5c047fe6567bf8910f72a728

## Raw Timing Table

Timing statistics are computed as the median of per-batch means, p95 of per-batch means, p99 of per-batch means, and max of per-batch means; they are not individual operation latency samples.

| Benchmark | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| SLH-DSA-SHA2-128s valid verify | available | 186167.910 | 187212.080 | 187965.831 | 188245.830 | 5374.392 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 186402.920 | 263561.040 | 315783.160 | 321121.660 | 4966.962 |
| BIP-340 Schnorr individual valid verify | available | 19884.015 | 20799.744 | 22229.094 | 23472.463 | 49843.726 |
| BIP-340 Schnorr individual invalid verify | available | 19832.044 | 19949.954 | 20111.952 | 20165.675 | 50411.268 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13545.243 | 14816.595 | 29190.907 | 32784.485 | 68900.480 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 175087.910 | 177175.000 | 177239.081 | 177248.330 | 5705.078 |
| middle_bitflip | available | 180093.330 | 183070.415 | 184358.751 | 184856.250 | 5547.301 |
| last_bitflip | available | 187644.590 | 190494.795 | 190585.752 | 190620.000 | 5324.689 |
| spread_32_bitflips | available | 195185.000 | 197559.585 | 198111.000 | 198281.250 | 5121.499 |
| all_zero | available | 186402.920 | 263561.040 | 315783.160 | 321121.660 | 4966.962 |
| all_ff | available | 184587.500 | 190786.665 | 204344.079 | 209992.080 | 5387.623 |
| deterministic_garbage | available | 188906.250 | 192695.625 | 195612.169 | 196517.920 | 5280.462 |
| wrong_message | available | 186060.420 | 189006.250 | 190623.581 | 190969.580 | 5367.345 |
| wrong_public_key | available | 188335.830 | 190259.375 | 193012.375 | 194113.750 | 5306.034 |
| min observed invalid fixed-length | available | 175087.910 | 177175.000 | 177239.081 | 177248.330 | 5705.078 |
| best observed invalid fixed-length | available | 175087.910 | 177175.000 | 177239.081 | 177248.330 | 5705.078 |
| median observed invalid fixed-length | available | 188335.830 | 190259.375 | 193012.375 | 194113.750 | 5306.034 |
| p99 observed invalid fixed-length | available | 186402.920 | 263561.040 | 315783.160 | 321121.660 | 4966.962 |
| worst observed invalid fixed-length | available | 186402.920 | 263561.040 | 315783.160 | 321121.660 | 4966.962 |

Worst observed invalid fixed-length case: `all_zero`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 353.306 | 362.876 | 365.964 | 367.110 | 2824936.460 |
| qrs_msg construction | available | 715.648 | 723.715 | 726.073 | 727.044 | 1396653.551 |
| verifier valid call | available | 187309.160 | 189681.465 | 190135.083 | 190312.080 | 5334.779 |
| total modeled valid path | available | 188927.500 | 190838.125 | 191485.294 | 191635.420 | 5294.460 |
| invalid structural reject | available | 2.717 | 3.274 | 3.553 | 3.627 | 355904163.045 |
| invalid fixed-length crypto reject | available | 185224.170 | 186819.790 | 187254.794 | 187415.420 | 5398.701 |

## Batch Schnorr Baseline

- `reviewed_public_api_status`: unavailable
- `reviewed_public_api_reason`: Bundled upstream bitcoin-core/secp256k1 commit aea86bc3509f3ceb5c047fe6567bf8910f72a728 exposes only the public secp256k1_schnorrsig_verify API for BIP-340; its schnorrsig tests still mark verify_batch as TODO, and git ls-remote found no upstream heads matching batch/schnorr/bip340 for a reviewed experimental baseline. No fake batch baseline is synthesized.
- `experimental_status`: available
- `experimental_label`: Experimental native BIP-340 batch baseline; not a reviewed public libsecp256k1 API.
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
| 64 | available | 1360121.583 | 1404230.533 | 21251.900 | 21941.102 |
| 128 | available | 2494923.667 | 2565680.533 | 19491.591 | 20044.379 |
| 512 | available | 8626326.417 | 9697471.533 | 16848.294 | 18940.374 |
| 2048 | available | 31103708.400 | 31397418.360 | 15187.358 | 15330.771 |
| 17391 | available | 235565312.500 | 507659062.300 | 13545.243 | 29190.907 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1427965.250 | 3446259.000 | 22311.957 | 53847.797 |
| 128 | available | 2517180.500 | 2555354.833 | 19665.473 | 19963.710 |
| 512 | available | 8768086.833 | 8978886.800 | 17125.170 | 17536.888 |
| 2048 | available | 31338716.800 | 31948069.920 | 15302.108 | 15599.644 |
| 17391 | available | 236017875.000 | 239496745.900 | 13571.265 | 13771.304 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 92.339 | 92.857 | 93.231 | 93.370 |
| QRS invalid fixed-length saturated block | available | 496 | 92.456 | 130.726 | 156.628 | 159.276 |
| Schnorr individual saturated block | available | 17391 | 345.803 | 361.728 | 386.586 | 408.210 |
| Schnorr experimental batch saturated block | available | 17391 | 235.565 | 257.675 | 507.659 | 570.155 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 93.231
- `experimental_batch_schnorr_p99_ms`: 507.659

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
