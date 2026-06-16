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
| SLH-DSA-SHA2-128s valid verify | available | 191171.875 | 199101.550 | 199657.810 | 199796.875 | 5248.380 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 194802.125 | 197390.050 | 197511.310 | 197541.625 | 5161.707 |
| BIP-340 Schnorr individual valid verify | available | 20035.375 | 20497.459 | 20592.409 | 20616.146 | 49768.585 |
| BIP-340 Schnorr individual invalid verify | available | 19996.000 | 20210.177 | 20250.702 | 20260.833 | 49935.637 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13397.483 | 13482.372 | 13488.487 | 13490.016 | 74673.534 |

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 358.916 | 374.721 | 376.911 | 377.458 | 2761591.237 |
| qrs_msg construction | available | 739.271 | 762.285 | 766.140 | 767.104 | 1351345.873 |
| verifier valid call | available | 187531.250 | 196092.175 | 196918.435 | 197125.000 | 5281.151 |
| total modeled valid path | available | 191567.625 | 199475.525 | 201286.805 | 201739.625 | 5209.322 |
| invalid structural reject | available | 2.688 | 6.865 | 8.290 | 8.646 | 283538561.244 |
| invalid fixed-length crypto reject | available | 178849.000 | 186446.875 | 187901.875 | 188265.625 | 5553.283 |

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
| 64 | available | 1347361.333 | 1374270.000 | 21052.521 | 21472.969 |
| 128 | available | 2507319.333 | 2538687.773 | 19588.432 | 19833.498 |
| 512 | available | 8548639.000 | 8779856.667 | 16696.561 | 17148.158 |
| 2048 | available | 30979000.000 | 31376548.960 | 15126.465 | 15320.581 |
| 17391 | available | 232995625.000 | 234578285.000 | 13397.483 | 13488.487 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1347500.000 | 1376397.320 | 21054.688 | 21506.208 |
| 128 | available | 2511555.667 | 2572178.333 | 19621.529 | 20095.143 |
| 512 | available | 8658180.333 | 8695465.560 | 16910.508 | 16983.331 |
| 2048 | available | 30793292.000 | 31025571.960 | 15035.787 | 15149.205 |
| 17391 | available | 232583125.000 | 235747216.960 | 13373.764 | 13555.702 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 94.821 | 98.754 | 99.030 | 99.099 |
| QRS invalid fixed-length saturated block | available | 496 | 96.622 | 97.905 | 97.966 | 97.981 |
| Schnorr individual saturated block | available | 17391 | 348.435 | 356.471 | 358.123 | 358.535 |
| Schnorr experimental batch saturated block | available | 17391 | 232.996 | 234.472 | 234.578 | 234.605 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 99.030
- `experimental_batch_schnorr_p99_ms`: 234.578

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
