# QRS Native Benchmark Report

Mode: `quick`

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
| SLH-DSA-SHA2-128s valid verify | available | 189104.125 | 193581.237 | 194553.747 | 194796.875 | 5356.097 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 188770.750 | 201670.775 | 204225.755 | 204864.500 | 5254.598 |
| BIP-340 Schnorr individual valid verify | available | 20101.041 | 20182.396 | 20190.929 | 20193.062 | 49785.733 |
| BIP-340 Schnorr individual invalid verify | available | 19970.209 | 20229.542 | 20285.492 | 20299.479 | 49997.093 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13777.974 | 13801.386 | 13805.686 | 13806.761 | 72853.773 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 187380.250 | 199870.312 | 203136.562 | 203953.125 | 5272.739 |
| middle_bitflip | available | 180088.500 | 191133.825 | 193360.065 | 193916.625 | 5489.815 |
| last_bitflip | available | 181062.500 | 193964.050 | 194055.310 | 194078.125 | 5448.001 |
| spread_32_bitflips | available | 186593.750 | 191994.187 | 192440.438 | 192552.000 | 5360.626 |
| all_zero | available | 188770.750 | 201670.775 | 204225.755 | 204864.500 | 5254.598 |
| all_ff | available | 189921.875 | 193269.875 | 194087.375 | 194291.750 | 5308.791 |
| deterministic_garbage | available | 165260.375 | 169396.400 | 169775.180 | 169869.875 | 6049.394 |
| wrong_message | available | 180786.375 | 189417.187 | 191408.438 | 191906.250 | 5557.693 |
| wrong_public_key | available | 177442.625 | 190346.875 | 191644.375 | 191968.750 | 5552.136 |
| min observed invalid fixed-length | available | 165260.375 | 169396.400 | 169775.180 | 169869.875 | 6049.394 |
| best observed invalid fixed-length | available | 165260.375 | 169396.400 | 169775.180 | 169869.875 | 6049.394 |
| median observed invalid fixed-length | available | 180088.500 | 191133.825 | 193360.065 | 193916.625 | 5489.815 |
| p99 observed invalid fixed-length | available | 188770.750 | 201670.775 | 204225.755 | 204864.500 | 5254.598 |
| worst observed invalid fixed-length | available | 188770.750 | 201670.775 | 204225.755 | 204864.500 | 5254.598 |

Worst observed invalid fixed-length case: `all_zero`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 348.354 | 364.044 | 368.509 | 369.625 | 2858917.720 |
| qrs_msg construction | available | 690.458 | 711.460 | 716.275 | 717.479 | 1438830.436 |
| verifier valid call | available | 183036.500 | 201116.663 | 202481.632 | 202822.875 | 5338.544 |
| total modeled valid path | available | 184625.000 | 191785.938 | 192307.188 | 192437.500 | 5401.776 |
| invalid structural reject | available | 2.646 | 2.675 | 2.685 | 2.688 | 377541664.419 |
| invalid fixed-length crypto reject | available | 180369.875 | 198054.212 | 201406.742 | 202244.875 | 5423.859 |

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
| 64 | available | 1388472.000 | 1438999.893 | 21694.875 | 22484.373 |
| 128 | available | 2572944.333 | 2608714.227 | 20101.128 | 20380.580 |
| 512 | available | 8689889.000 | 8768679.453 | 16972.439 | 17126.327 |
| 2048 | available | 31607250.000 | 32349074.680 | 15433.228 | 15795.447 |
| 17391 | available | 239612750.000 | 240094680.000 | 13777.974 | 13805.686 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1367527.667 | 1404377.213 | 21367.620 | 21943.394 |
| 128 | available | 2495750.000 | 2514027.560 | 19498.047 | 19640.840 |
| 512 | available | 8787347.000 | 8873944.893 | 17162.787 | 17331.924 |
| 2048 | available | 31201083.000 | 31596228.320 | 15234.904 | 15427.846 |
| 17391 | available | 260252666.000 | 275919313.960 | 14964.790 | 15865.638 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 93.796 | 96.016 | 96.499 | 96.619 |
| QRS invalid fixed-length saturated block | available | 496 | 93.630 | 100.029 | 101.296 | 101.613 |
| Schnorr individual saturated block | available | 17391 | 349.577 | 350.992 | 351.140 | 351.178 |
| Schnorr experimental batch saturated block | available | 17391 | 239.613 | 240.020 | 240.095 | 240.113 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 96.499
- `experimental_batch_schnorr_p99_ms`: 240.095

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
