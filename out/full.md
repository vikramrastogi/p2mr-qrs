# QRS Native Benchmark Report

Mode: `full`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_commit`: record with git rev-parse HEAD in published runs
- `build_type`: Release/NDEBUG
- `compiler`: clang 17.0.0 (clang-1700.6.4.2)
- `compiler_flags`: CMake target: -O3 -Wall -Wextra -Wpedantic
- `cpu_features`: hw.optional.arm.FEAT_AES=1, hw.optional.arm.FEAT_PMULL=1, hw.optional.arm.FEAT_SHA1=1, hw.optional.arm.FEAT_SHA256=1, hw.optional.arm.FEAT_SHA3=1, hw.optional.arm.FEAT_SHA512=1, hw.optional.arm.FEAT_LSE=1, hw.optional.arm.FEAT_FP16=1
- `cpu_frequency`: unavailable
- `cpu_model`: Apple M2 Pro
- `frequency_scaling`: not controlled by harness; reviewer must record governor/power mode
- `os`: Darwin 25.3.0 arm64
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
| SLH-DSA-SHA2-128s valid verify | available | 185599.580 | 189536.250 | 190363.160 | 190714.160 | 5380.685 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 188355.000 | 355001.665 | 415497.499 | 437196.250 | 4727.193 |
| BIP-340 Schnorr individual valid verify | available | 20507.183 | 24846.430 | 24994.470 | 25114.519 | 47609.132 |
| BIP-340 Schnorr individual invalid verify | available | 20604.992 | 24869.901 | 25948.874 | 26322.200 | 47019.390 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 51369.245 | 92705.250 | 244968.382 | 283034.165 | 15114.544 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 186760.830 | 190300.205 | 190707.125 | 190842.500 | 5345.160 |
| middle_bitflip | available | 179972.080 | 183133.750 | 187230.333 | 188789.580 | 5545.190 |
| last_bitflip | available | 188355.000 | 355001.665 | 415497.499 | 437196.250 | 4727.193 |
| spread_32_bitflips | available | 177743.750 | 181833.335 | 183136.714 | 183280.840 | 5621.682 |
| all_zero | available | 187799.170 | 191146.875 | 191997.874 | 192280.000 | 5327.562 |
| all_ff | available | 189817.920 | 193976.040 | 195179.874 | 195191.250 | 5272.426 |
| deterministic_garbage | available | 198231.250 | 290037.505 | 402504.752 | 436096.250 | 4680.748 |
| best observed invalid fixed-length | available | 177743.750 | 181833.335 | 183136.714 | 183280.840 | 5621.682 |
| median observed invalid fixed-length | available | 187799.170 | 191146.875 | 191997.874 | 192280.000 | 5327.562 |
| worst observed invalid fixed-length | available | 188355.000 | 355001.665 | 415497.499 | 437196.250 | 4727.193 |

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 1047.869 | 1078.275 | 1093.048 | 1098.037 | 950285.265 |
| qrs_msg construction | available | 1841.763 | 2153.873 | 2176.140 | 2185.477 | 519371.037 |
| verifier valid call | available | 436117.080 | 519976.455 | 530276.998 | 532705.000 | 2211.455 |
| total modeled valid path | available | 533967.090 | 542522.915 | 554450.087 | 559329.590 | 1933.375 |
| invalid structural reject | available | 6.375 | 6.384 | 7.246 | 7.615 | 155868695.206 |
| invalid fixed-length crypto reject | available | 493674.580 | 573818.540 | 577893.838 | 579014.590 | 1978.201 |

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
| 64 | available | 1418326.417 | 1527491.633 | 22161.350 | 23867.057 |
| 128 | available | 2601517.333 | 2637500.750 | 20324.354 | 20605.475 |
| 512 | available | 8895062.500 | 9150573.617 | 17373.169 | 17872.214 |
| 2048 | available | 32248016.600 | 49856648.360 | 15746.102 | 24344.067 |
| 17391 | available | 893362541.500 | 4260245133.200 | 51369.245 | 244968.382 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 5011631.917 | 6071706.900 | 78306.749 | 94870.420 |
| 128 | available | 8085385.417 | 8769490.983 | 63167.074 | 68511.648 |
| 512 | available | 27917093.750 | 63014236.117 | 54525.574 | 123074.680 |
| 2048 | available | 107610175.000 | 224662744.920 | 52544.031 | 109698.606 |
| 17391 | available | 758276062.500 | 4279842933.500 | 43601.637 | 246095.275 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 92.057 | 94.010 | 94.420 | 94.594 |
| QRS invalid fixed-length saturated block | available | 496 | 93.424 | 176.081 | 206.087 | 216.849 |
| Schnorr individual saturated block | available | 17391 | 356.640 | 432.104 | 434.679 | 436.767 |
| Schnorr experimental batch saturated block | available | 17391 | 893.363 | 1612.237 | 4260.245 | 4922.247 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 94.420
- `experimental_batch_schnorr_p99_ms`: 4260.245

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
