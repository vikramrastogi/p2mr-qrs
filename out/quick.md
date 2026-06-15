# QRS Native Benchmark Report

Mode: `quick`

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
| SLH-DSA-SHA2-128s valid verify | available | 186635.375 | 195066.188 | 195992.438 | 196224.000 | 5347.998 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 188505.125 | 195567.200 | 195963.440 | 196062.500 | 5253.428 |
| BIP-340 Schnorr individual valid verify | available | 20541.084 | 20862.875 | 20947.659 | 20968.855 | 48726.499 |
| BIP-340 Schnorr individual invalid verify | available | 20631.750 | 21110.167 | 21217.133 | 21243.875 | 48587.739 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13542.697 | 14025.823 | 14036.449 | 14039.105 | 73043.262 |

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 369.541 | 402.341 | 403.801 | 404.166 | 2670376.201 |
| qrs_msg construction | available | 729.833 | 776.417 | 787.167 | 789.854 | 1356649.355 |
| verifier valid call | available | 190739.625 | 215242.762 | 218777.752 | 219661.500 | 5068.121 |
| total modeled valid path | available | 193906.250 | 215426.075 | 220568.615 | 221854.250 | 5042.112 |
| invalid structural reject | available | 2.729 | 2.729 | 2.729 | 2.729 | 366809023.502 |
| invalid fixed-length crypto reject | available | 185791.625 | 208049.550 | 209880.810 | 210338.625 | 5250.184 |

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
| 64 | available | 1356625.000 | 1395440.000 | 21197.266 | 21803.750 |
| 128 | available | 2497416.667 | 2505670.560 | 19511.068 | 19575.551 |
| 512 | available | 8861694.667 | 9088068.787 | 17307.997 | 17750.134 |
| 2048 | available | 31562417.000 | 32443359.360 | 15411.336 | 15841.484 |
| 17391 | available | 235521041.000 | 244107884.680 | 13542.697 | 14036.449 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1352319.667 | 1364646.667 | 21129.995 | 21322.604 |
| 128 | available | 2464583.333 | 2506477.787 | 19254.557 | 19581.858 |
| 512 | available | 8788486.333 | 8925731.320 | 17165.012 | 17433.069 |
| 2048 | available | 31080333.000 | 31431231.960 | 15175.944 | 15347.281 |
| 17391 | available | 233677875.000 | 234983558.680 | 13436.713 | 13511.791 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 92.571 | 96.753 | 97.212 | 97.327 |
| QRS invalid fixed-length saturated block | available | 496 | 93.499 | 97.001 | 97.198 | 97.247 |
| Schnorr individual saturated block | available | 17391 | 357.230 | 362.826 | 364.301 | 364.669 |
| Schnorr experimental batch saturated block | available | 17391 | 235.521 | 243.923 | 244.108 | 244.154 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 97.212
- `experimental_batch_schnorr_p99_ms`: 244.108

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
