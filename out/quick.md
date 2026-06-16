# QRS Native Benchmark Report

Mode: `quick`

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
| SLH-DSA-SHA2-128s valid verify | available | 179484.375 | 182984.950 | 183126.190 | 183161.500 | 5537.792 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 185291.750 | 196546.325 | 199980.065 | 200838.500 | 5324.924 |
| BIP-340 Schnorr individual valid verify | available | 19619.750 | 20275.198 | 20494.123 | 20548.854 | 50665.872 |
| BIP-340 Schnorr individual invalid verify | available | 19576.229 | 19671.489 | 19703.981 | 19712.104 | 51061.134 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13536.837 | 13818.613 | 13872.858 | 13886.419 | 73655.512 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 174531.250 | 178623.987 | 178816.497 | 178864.625 | 5702.672 |
| middle_bitflip | available | 185291.750 | 196546.325 | 199980.065 | 200838.500 | 5324.924 |
| last_bitflip | available | 179380.250 | 190291.087 | 193937.318 | 194848.875 | 5506.414 |
| spread_32_bitflips | available | 167359.375 | 170722.413 | 171148.682 | 171255.250 | 5944.904 |
| all_zero | available | 173260.375 | 185434.387 | 188061.878 | 188718.750 | 5670.934 |
| all_ff | available | 175973.875 | 186733.288 | 190100.758 | 190942.625 | 5642.460 |
| deterministic_garbage | available | 183947.875 | 193924.987 | 196847.497 | 197578.125 | 5373.808 |
| wrong_message | available | 184515.625 | 189003.112 | 189113.122 | 189140.625 | 5382.912 |
| wrong_public_key | available | 180198.000 | 186954.237 | 187649.247 | 187823.000 | 5527.610 |
| min observed invalid fixed-length | available | 167359.375 | 170722.413 | 171148.682 | 171255.250 | 5944.904 |
| best observed invalid fixed-length | available | 167359.375 | 170722.413 | 171148.682 | 171255.250 | 5944.904 |
| median observed invalid fixed-length | available | 184515.625 | 189003.112 | 189113.122 | 189140.625 | 5382.912 |
| p99 observed invalid fixed-length | available | 185291.750 | 196546.325 | 199980.065 | 200838.500 | 5324.924 |
| worst observed invalid fixed-length | available | 185291.750 | 196546.325 | 199980.065 | 200838.500 | 5324.924 |

Worst observed invalid fixed-length case: `middle_bitflip`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 355.791 | 363.719 | 364.394 | 364.562 | 2791555.544 |
| qrs_msg construction | available | 718.333 | 741.140 | 743.245 | 743.771 | 1386235.788 |
| verifier valid call | available | 186697.875 | 197160.925 | 198482.185 | 198812.500 | 5341.643 |
| total modeled valid path | available | 184140.625 | 188378.125 | 188588.125 | 188640.625 | 5413.330 |
| invalid structural reject | available | 2.729 | 5.177 | 6.002 | 6.208 | 310249307.479 |
| invalid fixed-length crypto reject | available | 196083.375 | 208871.875 | 209674.375 | 209875.000 | 5027.212 |

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
| 64 | available | 1348458.333 | 1388185.000 | 21069.661 | 21690.391 |
| 128 | available | 2516500.000 | 2561374.547 | 19660.156 | 20010.739 |
| 512 | available | 8678763.667 | 8727617.107 | 16950.710 | 17046.127 |
| 2048 | available | 30850750.000 | 31015520.000 | 15063.843 | 15144.297 |
| 17391 | available | 235419125.000 | 241262866.320 | 13536.837 | 13872.858 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1345972.333 | 1378295.347 | 21030.818 | 21535.865 |
| 128 | available | 2506819.333 | 2521788.787 | 19584.526 | 19701.475 |
| 512 | available | 8620680.667 | 8659802.680 | 16837.267 | 16913.677 |
| 2048 | available | 30934292.000 | 31215520.640 | 15104.635 | 15241.953 |
| 17391 | available | 234022167.000 | 234819759.000 | 13456.510 | 13502.372 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 89.024 | 90.761 | 90.831 | 90.848 |
| QRS invalid fixed-length saturated block | available | 496 | 91.905 | 97.487 | 99.190 | 99.616 |
| Schnorr individual saturated block | available | 17391 | 341.207 | 352.606 | 356.413 | 357.365 |
| Schnorr experimental batch saturated block | available | 17391 | 235.419 | 240.319 | 241.263 | 241.499 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 90.831
- `experimental_batch_schnorr_p99_ms`: 241.263

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
