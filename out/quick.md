# QRS Native Benchmark Report

Mode: `quick`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: 3821510b6bcc223e0d716dc65021f249afca3e68
- `benchmark_schema`: qrs-native-bench/v0
- `build_type`: Release/NDEBUG
- `cmake_build_type`: Release
- `compiler`: clang
- `compiler_flags`: -O3 -Wall -Wextra -Wpedantic
- `compiler_version`: 17.0.0 (clang-1700.6.4.2)
- `cpu_features`: hw.optional.arm.FEAT_AES=1, hw.optional.arm.FEAT_PMULL=1, hw.optional.arm.FEAT_SHA1=1, hw.optional.arm.FEAT_SHA256=1, hw.optional.arm.FEAT_SHA3=1, hw.optional.arm.FEAT_SHA512=1, hw.optional.arm.FEAT_LSE=1, hw.optional.arm.FEAT_FP16=1
- `cpu_frequency`: unavailable
- `cpu_model`: Apple M2 Pro
- `frequency_scaling`: not controlled by harness; reviewer must record governor/power mode
- `git_commit`: 3821510b6bcc223e0d716dc65021f249afca3e68
- `openssl_provider`: default
- `openssl_slh_dsa_algorithm`: SLH-DSA-SHA2-128s
- `openssl_slh_dsa_mode`: EVP direct message signing/verification, no prehash digest, context-string explicitly set to empty
- `openssl_version`: OpenSSL 3.6.1 27 Jan 2026
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
| SLH-DSA-SHA2-128s valid verify | available | 197739.625 | 202087.475 | 202779.995 | 202953.125 | 5097.803 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 191666.625 | 218967.737 | 226097.747 | 227880.250 | 5062.300 |
| BIP-340 Schnorr individual valid verify | available | 19727.250 | 19960.292 | 20018.792 | 20033.416 | 50592.963 |
| BIP-340 Schnorr individual invalid verify | available | 20007.770 | 20508.416 | 20587.433 | 20607.188 | 49830.774 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13528.981 | 13647.294 | 13651.543 | 13652.605 | 73697.447 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 191666.625 | 218967.737 | 226097.747 | 227880.250 | 5062.300 |
| middle_bitflip | available | 193390.625 | 210842.762 | 215322.752 | 216442.750 | 5129.751 |
| last_bitflip | available | 183812.500 | 195838.513 | 198038.503 | 198588.500 | 5356.332 |
| spread_32_bitflips | available | 186932.375 | 202425.562 | 206126.812 | 207052.125 | 5261.035 |
| all_zero | available | 179395.875 | 188956.262 | 190503.753 | 190890.625 | 5536.172 |
| all_ff | available | 175536.375 | 184730.787 | 187204.557 | 187823.000 | 5631.844 |
| deterministic_garbage | available | 185078.125 | 192636.488 | 194568.997 | 195052.125 | 5373.701 |
| wrong_message | available | 186421.875 | 197169.288 | 198638.057 | 199005.250 | 5302.716 |
| wrong_public_key | available | 176859.375 | 188117.675 | 189852.635 | 190286.375 | 5567.984 |
| min observed invalid fixed-length | available | 175536.375 | 184730.787 | 187204.557 | 187823.000 | 5631.844 |
| best observed invalid fixed-length | available | 175536.375 | 184730.787 | 187204.557 | 187823.000 | 5631.844 |
| median observed invalid fixed-length | available | 183812.500 | 195838.513 | 198038.503 | 198588.500 | 5356.332 |
| p99 observed invalid fixed-length | available | 191666.625 | 218967.737 | 226097.747 | 227880.250 | 5062.300 |
| worst observed invalid fixed-length | available | 191666.625 | 218967.737 | 226097.747 | 227880.250 | 5062.300 |

Worst observed invalid fixed-length case: `byte0_bitflip`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 348.292 | 356.919 | 359.484 | 360.125 | 2866649.016 |
| qrs_msg construction | available | 700.104 | 762.323 | 778.298 | 782.292 | 1400630.284 |
| verifier valid call | available | 174703.125 | 183729.162 | 186466.633 | 187151.000 | 5681.217 |
| total modeled valid path | available | 180791.625 | 189259.362 | 191101.872 | 191562.500 | 5552.939 |
| invalid structural reject | available | 2.646 | 2.646 | 2.646 | 2.646 | 378388605.097 |
| invalid fixed-length crypto reject | available | 169927.125 | 177160.938 | 177382.188 | 177437.500 | 5831.561 |

## Batch Schnorr Baseline

- `reviewed_public_api_status`: unavailable
- `reviewed_public_api_reason`: Bundled upstream bitcoin-core/secp256k1 commit aea86bc3509f3ceb5c047fe6567bf8910f72a728 exposes only the public secp256k1_schnorrsig_verify API for BIP-340; its schnorrsig tests still mark verify_batch as TODO. The separate scripts/check_batch_schnorr_baseline.py evidence report records local API status and optional upstream-head lookup status. No fake batch baseline is synthesized.
- `experimental_status`: available
- `experimental_label`: Experimental native BIP-340 batch baseline; not a reviewed public libsecp256k1 API.
- BIP-340 challenge self-test: `available` - BIP-340 challenge self-test passed: libsecp256k1 tagged SHA helper matched an independent tagged_hash construction over deterministic inputs and over the R || P || m tuple from a generated valid signature.
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
| 64 | available | 1329875.000 | 1375964.440 | 20779.297 | 21499.444 |
| 128 | available | 2478055.667 | 2545907.427 | 19359.810 | 19889.902 |
| 512 | available | 8635388.667 | 8858785.560 | 16865.993 | 17302.316 |
| 2048 | available | 31225125.000 | 31306820.320 | 15246.643 | 15286.533 |
| 17391 | available | 235282500.000 | 237413984.680 | 13528.981 | 13651.543 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1346166.667 | 1409160.907 | 21033.854 | 22018.139 |
| 128 | available | 2502333.333 | 2593958.440 | 19549.479 | 20265.300 |
| 512 | available | 8648625.000 | 8679284.000 | 16891.846 | 16951.727 |
| 2048 | available | 31251917.000 | 31268475.000 | 15259.725 | 15267.810 |
| 17391 | available | 235997959.000 | 237395688.960 | 13570.120 | 13650.491 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 98.079 | 100.235 | 100.579 | 100.665 |
| QRS invalid fixed-length saturated block | available | 496 | 95.067 | 108.608 | 112.144 | 113.029 |
| Schnorr individual saturated block | available | 17391 | 343.077 | 347.129 | 348.147 | 348.401 |
| Schnorr experimental batch saturated block | available | 17391 | 235.282 | 237.340 | 237.414 | 237.432 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 100.579
- `experimental_batch_schnorr_p99_ms`: 237.414

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
