# QRS Native Benchmark Report

Mode: `quick`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: f09a5053a61b704bd8eb9b9742122b0ddb625cd5
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
- `git_commit`: f09a5053a61b704bd8eb9b9742122b0ddb625cd5
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

## SLH-DSA Backend Status

| Backend | Status | Provider | Version | Public key bytes | Signature bytes | Reason |
|---|---:|---|---|---:|---:|---|
| openssl | available | default | OpenSSL 3.6.1 27 Jan 2026 | 32 | 7856 |  |
| second_reviewed_backend | unavailable | unavailable | unavailable | 32 | 7856 | second reviewed SLH-DSA backend not wired |

The second reviewed SLH-DSA backend is reported separately from the OpenSSL backend; it remains unavailable until a reviewed/pinned verifier strategy is wired for cross-backend agreement.

## Raw Timing Table

Timing statistics are computed as the median of per-batch means, p95 of per-batch means, p99 of per-batch means, and max of per-batch means; they are not individual operation latency samples.

| Benchmark | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| SLH-DSA-SHA2-128s valid verify | available | 182635.500 | 187142.725 | 187445.245 | 187520.875 | 5488.313 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 194156.250 | 203410.375 | 205315.375 | 205791.625 | 5094.904 |
| BIP-340 Schnorr individual valid verify | available | 20510.791 | 20647.479 | 20657.546 | 20660.062 | 48785.761 |
| BIP-340 Schnorr individual invalid verify | available | 20517.771 | 20683.667 | 20688.233 | 20689.375 | 48856.793 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 14107.666 | 14217.315 | 14233.966 | 14238.128 | 70873.213 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 197224.000 | 201609.362 | 202109.373 | 202234.375 | 5091.835 |
| middle_bitflip | available | 178088.500 | 184117.737 | 185577.747 | 185942.750 | 5586.360 |
| last_bitflip | available | 181281.250 | 188377.138 | 190404.628 | 190911.500 | 5517.150 |
| spread_32_bitflips | available | 192609.375 | 194975.025 | 194995.005 | 195000.000 | 5204.420 |
| all_zero | available | 174781.250 | 180351.100 | 180728.620 | 180823.000 | 5681.049 |
| all_ff | available | 182369.750 | 187511.488 | 188393.997 | 188614.625 | 5450.342 |
| deterministic_garbage | available | 186312.500 | 188986.513 | 189476.503 | 189599.000 | 5379.658 |
| wrong_message | available | 194156.250 | 203410.375 | 205315.375 | 205791.625 | 5094.904 |
| wrong_public_key | available | 185828.125 | 191363.062 | 191501.812 | 191536.500 | 5395.207 |
| min observed invalid fixed-length | available | 174781.250 | 180351.100 | 180728.620 | 180823.000 | 5681.049 |
| best observed invalid fixed-length | available | 174781.250 | 180351.100 | 180728.620 | 180823.000 | 5681.049 |
| median observed invalid fixed-length | available | 181281.250 | 188377.138 | 190404.628 | 190911.500 | 5517.150 |
| p99 observed invalid fixed-length | available | 194156.250 | 203410.375 | 205315.375 | 205791.625 | 5094.904 |
| worst observed invalid fixed-length | available | 194156.250 | 203410.375 | 205315.375 | 205791.625 | 5094.904 |

Worst observed invalid fixed-length case: `wrong_message`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 361.709 | 363.562 | 363.612 | 363.625 | 2761272.600 |
| qrs_msg construction | available | 749.333 | 756.054 | 756.594 | 756.729 | 1343892.747 |
| verifier valid call | available | 194421.875 | 198816.737 | 199871.747 | 200135.500 | 5168.017 |
| total modeled valid path | available | 194031.250 | 198582.263 | 199274.753 | 199447.875 | 5168.118 |
| invalid structural reject | available | 2.792 | 2.842 | 2.852 | 2.854 | 358202845.154 |
| invalid fixed-length crypto reject | available | 200755.125 | 201804.688 | 202123.438 | 202203.125 | 5012.251 |

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
| 64 | available | 1397236.000 | 1429869.547 | 21831.812 | 22341.712 |
| 128 | available | 2639639.000 | 2651317.000 | 20622.180 | 20713.414 |
| 512 | available | 9072278.000 | 9200320.440 | 17719.293 | 17969.376 |
| 2048 | available | 32222166.000 | 32774026.040 | 15733.479 | 16002.942 |
| 17391 | available | 245346417.000 | 247542897.720 | 14107.666 | 14233.966 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1419014.000 | 1428810.000 | 22172.094 | 22325.156 |
| 128 | available | 2649597.333 | 2651593.227 | 20699.979 | 20715.572 |
| 512 | available | 8845194.667 | 8887682.120 | 17275.771 | 17358.754 |
| 2048 | available | 32822916.000 | 34133151.680 | 16026.814 | 16666.578 |
| 17391 | available | 244919792.000 | 257296751.640 | 14083.134 | 14794.822 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs. QRS depth rows include the control-block witness weight for the Merkle path and report the resulting TapBranch hash-operation count; timing estimates still use the measured QRS validation-path timing multiplied by the depth-specific weight-limited input count.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 90.587 | 92.823 | 92.973 | 93.010 |
| QRS invalid fixed-length saturated block | available | 496 | 96.302 | 100.892 | 101.836 | 102.073 |
| Schnorr individual saturated block | available | 17391 | 356.703 | 359.080 | 359.255 | 359.299 |
| Schnorr experimental batch saturated block | available | 17391 | 245.346 | 247.253 | 247.543 | 247.615 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS depth-sensitive saturated block estimates

| Model | QRS Merkle depth | Status | Input WU | Max inputs | TapBranch hashes/block | Median ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | 0 | available | 8059 | 496 | 0 | 90.587 | 92.973 | 93.010 |
| QRS valid saturated block | 1 | available | 8091 | 494 | 494 | 90.222 | 92.598 | 92.635 |
| QRS valid saturated block | 3 | available | 8155 | 490 | 1470 | 89.491 | 91.848 | 91.885 |
| QRS valid saturated block | 128 | available | 12155 | 329 | 42112 | 60.087 | 61.669 | 61.694 |
| QRS invalid fixed-length saturated block | 0 | available | 8059 | 496 | 0 | 96.302 | 101.836 | 102.073 |
| QRS invalid fixed-length saturated block | 1 | available | 8091 | 494 | 494 | 95.913 | 101.426 | 101.661 |
| QRS invalid fixed-length saturated block | 3 | available | 8155 | 490 | 1470 | 95.137 | 100.605 | 100.838 |
| QRS invalid fixed-length saturated block | 128 | available | 12155 | 329 | 42112 | 63.877 | 67.549 | 67.705 |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 92.973
- `experimental_batch_schnorr_p99_ms`: 247.543

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
