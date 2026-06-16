# QRS Native Benchmark Report

Mode: `full`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: b1bcd3bdb3211ff9f69263bbf0d228c8e8969b53
- `build_type`: Release/NDEBUG
- `compiler`: clang 17.0.0 (clang-1700.6.4.2)
- `compiler_flags`: CMake target: -O3 -Wall -Wextra -Wpedantic
- `cpu_features`: hw.optional.arm.FEAT_AES=1, hw.optional.arm.FEAT_PMULL=1, hw.optional.arm.FEAT_SHA1=1, hw.optional.arm.FEAT_SHA256=1, hw.optional.arm.FEAT_SHA3=1, hw.optional.arm.FEAT_SHA512=1, hw.optional.arm.FEAT_LSE=1, hw.optional.arm.FEAT_FP16=1
- `cpu_frequency`: unavailable
- `cpu_model`: Apple M2 Pro
- `frequency_scaling`: not controlled by harness; reviewer must record governor/power mode
- `git_commit`: b1bcd3bdb3211ff9f69263bbf0d228c8e8969b53
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
| SLH-DSA-SHA2-128s valid verify | available | 181310.830 | 184681.460 | 185593.039 | 185799.160 | 5500.861 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 189964.580 | 203158.750 | 252565.796 | 270852.920 | 5179.159 |
| BIP-340 Schnorr individual valid verify | available | 19866.823 | 20182.889 | 20263.297 | 20304.477 | 50233.010 |
| BIP-340 Schnorr individual invalid verify | available | 19878.317 | 19996.454 | 20761.859 | 21415.271 | 50219.967 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13647.706 | 13838.389 | 13880.292 | 13890.768 | 73222.124 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 186129.580 | 189380.630 | 190795.626 | 191130.000 | 5376.042 |
| middle_bitflip | available | 189964.580 | 203158.750 | 252565.796 | 270852.920 | 5179.159 |
| last_bitflip | available | 182804.580 | 184325.625 | 184373.376 | 184387.500 | 5480.802 |
| spread_32_bitflips | available | 191234.590 | 193588.540 | 194872.204 | 195384.580 | 5225.935 |
| all_zero | available | 175570.000 | 179026.875 | 180295.543 | 180467.920 | 5678.566 |
| all_ff | available | 188579.580 | 190276.250 | 191865.127 | 192377.500 | 5304.786 |
| deterministic_garbage | available | 193475.000 | 196191.250 | 196499.919 | 196522.920 | 5168.420 |
| min observed invalid fixed-length | available | 175570.000 | 179026.875 | 180295.543 | 180467.920 | 5678.566 |
| best observed invalid fixed-length | available | 175570.000 | 179026.875 | 180295.543 | 180467.920 | 5678.566 |
| median observed invalid fixed-length | available | 188579.580 | 190276.250 | 191865.127 | 192377.500 | 5304.786 |
| p99 observed invalid fixed-length | available | 189964.580 | 203158.750 | 252565.796 | 270852.920 | 5179.159 |
| worst observed invalid fixed-length | available | 189964.580 | 203158.750 | 252565.796 | 270852.920 | 5179.159 |

Worst observed invalid fixed-length case: `middle_bitflip`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 362.196 | 373.751 | 384.504 | 387.231 | 2752302.737 |
| qrs_msg construction | available | 737.731 | 756.101 | 758.336 | 759.033 | 1354400.571 |
| verifier valid call | available | 192676.250 | 208981.665 | 238296.667 | 248631.670 | 5107.205 |
| total modeled valid path | available | 193200.830 | 199499.790 | 199975.668 | 200036.670 | 5160.226 |
| invalid structural reject | available | 2.721 | 3.269 | 3.608 | 3.683 | 360186390.648 |
| invalid fixed-length crypto reject | available | 186429.590 | 191615.000 | 192637.582 | 192962.080 | 5346.106 |

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
| 64 | available | 1347885.417 | 1365820.867 | 21060.710 | 21340.951 |
| 128 | available | 2509989.583 | 2537542.367 | 19609.294 | 19824.550 |
| 512 | available | 8691902.833 | 8757823.633 | 16976.373 | 17105.124 |
| 2048 | available | 31266800.000 | 31438473.320 | 15266.992 | 15350.817 |
| 17391 | available | 237347250.000 | 241392166.500 | 13647.706 | 13880.292 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1338513.833 | 1388169.433 | 20914.279 | 21690.147 |
| 128 | available | 2496059.000 | 2546741.700 | 19500.461 | 19896.420 |
| 512 | available | 8674204.833 | 8932888.917 | 16941.806 | 17447.049 |
| 2048 | available | 31487250.000 | 32113633.320 | 15374.634 | 15680.485 |
| 17391 | available | 239836646.000 | 299907229.100 | 13790.848 | 17244.967 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 89.930 | 91.602 | 92.054 | 92.156 |
| QRS invalid fixed-length saturated block | available | 496 | 94.222 | 100.767 | 125.273 | 134.343 |
| Schnorr individual saturated block | available | 17391 | 345.504 | 351.001 | 352.399 | 353.115 |
| Schnorr experimental batch saturated block | available | 17391 | 237.347 | 240.663 | 241.392 | 241.574 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 92.054
- `experimental_batch_schnorr_p99_ms`: 241.392

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
