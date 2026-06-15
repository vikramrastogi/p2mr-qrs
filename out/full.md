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
| SLH-DSA-SHA2-128s valid verify | available | 178257.920 | 181601.665 | 182584.037 | 182689.160 | 5602.341 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 185582.920 | 189320.000 | 191321.044 | 192152.920 | 5381.673 |
| BIP-340 Schnorr individual valid verify | available | 20508.706 | 22090.888 | 22833.556 | 23140.944 | 48307.107 |
| BIP-340 Schnorr individual invalid verify | available | 20576.767 | 21398.389 | 21734.239 | 21744.737 | 48407.570 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13643.866 | 13838.895 | 14187.578 | 14274.748 | 73130.634 |

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 370.452 | 383.850 | 412.810 | 423.508 | 2690031.318 |
| qrs_msg construction | available | 746.217 | 872.605 | 982.309 | 1003.952 | 1312465.818 |
| verifier valid call | available | 183040.420 | 185108.745 | 185734.329 | 185969.580 | 5462.305 |
| total modeled valid path | available | 184461.670 | 187558.545 | 188443.706 | 188779.580 | 5406.353 |
| invalid structural reject | available | 2.721 | 2.730 | 2.863 | 2.919 | 366610315.113 |
| invalid fixed-length crypto reject | available | 180726.250 | 184076.670 | 185692.044 | 186202.920 | 5538.814 |

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
| 64 | available | 1377691.000 | 1402218.750 | 21526.422 | 21909.668 |
| 128 | available | 2546038.250 | 2636993.100 | 19890.924 | 20601.509 |
| 512 | available | 8845663.250 | 8942602.067 | 17276.686 | 17466.020 |
| 2048 | available | 31546658.400 | 35930859.920 | 15403.642 | 17544.365 |
| 17391 | available | 237280479.500 | 246736162.600 | 13643.866 | 14187.578 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1350413.250 | 1401334.033 | 21100.207 | 21895.844 |
| 128 | available | 2527555.583 | 2588505.533 | 19746.528 | 20222.699 |
| 512 | available | 8838812.500 | 11340077.100 | 17263.306 | 22148.588 |
| 2048 | available | 31314725.000 | 33009766.600 | 15290.393 | 16118.050 |
| 17391 | available | 236972583.000 | 244468370.600 | 13626.162 | 14057.177 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 88.416 | 90.074 | 90.562 | 90.614 |
| QRS invalid fixed-length saturated block | available | 496 | 92.049 | 93.903 | 94.895 | 95.308 |
| Schnorr individual saturated block | available | 17391 | 356.667 | 384.183 | 397.098 | 402.444 |
| Schnorr experimental batch saturated block | available | 17391 | 237.280 | 240.672 | 246.736 | 248.252 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 90.562
- `experimental_batch_schnorr_p99_ms`: 246.736

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
