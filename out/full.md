# QRS Native Benchmark Report

Mode: `full`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: 34a1a1deb57f5c80bb1a893dca8265183a55e84b
- `build_type`: Release/NDEBUG
- `compiler`: clang 17.0.0 (clang-1700.6.4.2)
- `compiler_flags`: CMake target: -O3 -Wall -Wextra -Wpedantic
- `cpu_features`: hw.optional.arm.FEAT_AES=1, hw.optional.arm.FEAT_PMULL=1, hw.optional.arm.FEAT_SHA1=1, hw.optional.arm.FEAT_SHA256=1, hw.optional.arm.FEAT_SHA3=1, hw.optional.arm.FEAT_SHA512=1, hw.optional.arm.FEAT_LSE=1, hw.optional.arm.FEAT_FP16=1
- `cpu_frequency`: unavailable
- `cpu_model`: Apple M2 Pro
- `frequency_scaling`: not controlled by harness; reviewer must record governor/power mode
- `git_commit`: 34a1a1deb57f5c80bb1a893dca8265183a55e84b
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
| SLH-DSA-SHA2-128s valid verify | available | 177911.250 | 180996.875 | 181459.750 | 181626.250 | 5619.610 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 175700.000 | 180009.795 | 207922.963 | 219865.840 | 5635.045 |
| BIP-340 Schnorr individual valid verify | available | 19911.040 | 20262.627 | 20700.023 | 20753.069 | 50055.497 |
| BIP-340 Schnorr individual invalid verify | available | 20002.010 | 22278.752 | 23913.907 | 24068.233 | 49298.851 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13716.156 | 14944.449 | 16452.751 | 16829.827 | 71437.107 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 193024.590 | 195923.330 | 196848.667 | 197024.170 | 5176.670 |
| middle_bitflip | available | 191395.000 | 194853.755 | 198061.082 | 198960.830 | 5211.845 |
| last_bitflip | available | 177667.920 | 180199.790 | 180392.543 | 180470.420 | 5623.614 |
| spread_32_bitflips | available | 183899.590 | 186057.080 | 187298.000 | 187576.250 | 5440.856 |
| all_zero | available | 174074.580 | 176680.625 | 177524.375 | 177530.000 | 5740.304 |
| all_ff | available | 175700.000 | 180009.795 | 207922.963 | 219865.840 | 5635.045 |
| deterministic_garbage | available | 177909.580 | 180395.620 | 182856.998 | 183837.500 | 5619.186 |
| wrong_message | available | 189207.920 | 192189.375 | 192732.662 | 192780.410 | 5280.785 |
| wrong_public_key | available | 185199.170 | 187595.625 | 189611.543 | 190224.170 | 5400.212 |
| min observed invalid fixed-length | available | 174074.580 | 176680.625 | 177524.375 | 177530.000 | 5740.304 |
| best observed invalid fixed-length | available | 174074.580 | 176680.625 | 177524.375 | 177530.000 | 5740.304 |
| median observed invalid fixed-length | available | 185199.170 | 187595.625 | 189611.543 | 190224.170 | 5400.212 |
| p99 observed invalid fixed-length | available | 175700.000 | 180009.795 | 207922.963 | 219865.840 | 5635.045 |
| worst observed invalid fixed-length | available | 175700.000 | 180009.795 | 207922.963 | 219865.840 | 5635.045 |

Worst observed invalid fixed-length case: `all_ff`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 353.979 | 363.301 | 366.399 | 367.250 | 2824671.591 |
| qrs_msg construction | available | 707.467 | 714.785 | 718.218 | 719.302 | 1414412.762 |
| verifier valid call | available | 180327.500 | 182437.080 | 183317.706 | 183652.080 | 5535.054 |
| total modeled valid path | available | 182207.920 | 184139.585 | 185389.207 | 185900.830 | 5487.063 |
| invalid structural reject | available | 2.642 | 2.820 | 3.110 | 3.233 | 372942201.177 |
| invalid fixed-length crypto reject | available | 181084.580 | 182339.165 | 182525.625 | 182587.500 | 5526.470 |

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
| 64 | available | 1443864.583 | 2030029.167 | 22560.384 | 31719.206 |
| 128 | available | 2805079.917 | 3952223.600 | 21914.687 | 30876.747 |
| 512 | available | 8696628.500 | 10930236.800 | 16985.603 | 21348.119 |
| 2048 | available | 31599750.000 | 36646516.720 | 15429.565 | 17893.807 |
| 17391 | available | 238537667.000 | 286129799.800 | 13716.156 | 16452.751 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1340215.250 | 1380469.450 | 20940.863 | 21569.835 |
| 128 | available | 2500524.250 | 2611060.367 | 19535.346 | 20398.909 |
| 512 | available | 8709128.417 | 8836867.350 | 17010.016 | 17259.507 |
| 2048 | available | 31374691.600 | 35792411.720 | 15319.674 | 17476.764 |
| 17391 | available | 238998687.500 | 276132966.500 | 13742.665 | 15877.923 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 88.244 | 89.774 | 90.004 | 90.087 |
| QRS invalid fixed-length saturated block | available | 496 | 87.147 | 89.285 | 103.130 | 109.053 |
| Schnorr individual saturated block | available | 17391 | 346.273 | 352.387 | 359.994 | 360.917 |
| Schnorr experimental batch saturated block | available | 17391 | 238.538 | 259.899 | 286.130 | 292.688 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 90.004
- `experimental_batch_schnorr_p99_ms`: 286.130

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
