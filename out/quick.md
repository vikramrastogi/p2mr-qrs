# QRS Native Benchmark Report

Mode: `quick`

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
| SLH-DSA-SHA2-128s valid verify | available | 189671.875 | 190528.663 | 190684.932 | 190724.000 | 5309.672 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 190312.500 | 196701.025 | 198386.005 | 198807.250 | 5235.623 |
| BIP-340 Schnorr individual valid verify | available | 20237.021 | 20457.000 | 20463.350 | 20464.938 | 49320.037 |
| BIP-340 Schnorr individual invalid verify | available | 20302.375 | 20434.500 | 20470.633 | 20479.666 | 49355.287 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13752.393 | 13862.917 | 13873.034 | 13875.563 | 72559.094 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 189916.625 | 193344.212 | 193660.443 | 193739.500 | 5256.263 |
| middle_bitflip | available | 185572.875 | 193021.325 | 194175.065 | 194463.500 | 5359.835 |
| last_bitflip | available | 190312.500 | 196701.025 | 198386.005 | 198807.250 | 5235.623 |
| spread_32_bitflips | available | 175541.750 | 183633.350 | 184243.370 | 184395.875 | 5618.729 |
| all_zero | available | 180849.000 | 194809.900 | 197153.680 | 197739.625 | 5449.282 |
| all_ff | available | 178192.625 | 189122.400 | 189553.680 | 189661.500 | 5515.861 |
| deterministic_garbage | available | 186578.125 | 191720.837 | 193085.867 | 193427.125 | 5336.765 |
| min observed invalid fixed-length | available | 175541.750 | 183633.350 | 184243.370 | 184395.875 | 5618.729 |
| best observed invalid fixed-length | available | 175541.750 | 183633.350 | 184243.370 | 184395.875 | 5618.729 |
| median observed invalid fixed-length | available | 189916.625 | 193344.212 | 193660.443 | 193739.500 | 5256.263 |
| p99 observed invalid fixed-length | available | 190312.500 | 196701.025 | 198386.005 | 198807.250 | 5235.623 |
| worst observed invalid fixed-length | available | 190312.500 | 196701.025 | 198386.005 | 198807.250 | 5235.623 |

Worst observed invalid fixed-length case: `last_bitflip`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 359.188 | 373.489 | 376.714 | 377.521 | 2764886.494 |
| qrs_msg construction | available | 725.312 | 741.033 | 743.123 | 743.646 | 1392936.558 |
| verifier valid call | available | 186442.625 | 193716.087 | 195584.817 | 196052.000 | 5342.238 |
| total modeled valid path | available | 188901.000 | 194331.788 | 194720.557 | 194817.750 | 5276.278 |
| invalid structural reject | available | 2.729 | 3.931 | 4.336 | 4.438 | 336005376.086 |
| invalid fixed-length crypto reject | available | 197901.000 | 203619.300 | 204378.060 | 204567.750 | 5019.027 |

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
| 64 | available | 1360638.667 | 1407063.773 | 21259.979 | 21985.371 |
| 128 | available | 2568430.667 | 2599988.787 | 20065.865 | 20312.412 |
| 512 | available | 8882416.667 | 9068830.000 | 17348.470 | 17712.559 |
| 2048 | available | 31769334.000 | 32742356.640 | 15512.370 | 15987.479 |
| 17391 | available | 239167875.000 | 241265931.960 | 13752.393 | 13873.034 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1365500.000 | 1393754.893 | 21335.938 | 21777.420 |
| 128 | available | 2572319.333 | 2618805.440 | 20096.245 | 20459.417 |
| 512 | available | 8835375.000 | 8869407.773 | 17256.592 | 17323.062 |
| 2048 | available | 31908041.000 | 32042458.960 | 15580.098 | 15645.732 |
| 17391 | available | 240114583.000 | 241190326.640 | 13806.830 | 13868.686 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 94.077 | 94.502 | 94.580 | 94.599 |
| QRS invalid fixed-length saturated block | available | 496 | 94.395 | 97.564 | 98.399 | 98.608 |
| Schnorr individual saturated block | available | 17391 | 351.942 | 355.768 | 355.878 | 355.906 |
| Schnorr experimental batch saturated block | available | 17391 | 239.168 | 241.090 | 241.266 | 241.310 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 94.580
- `experimental_batch_schnorr_p99_ms`: 241.266

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
