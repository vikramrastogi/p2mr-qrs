# QRS Native Benchmark Report

Mode: `standard`

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
- `working_tree_dirty`: true
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
| SLH-DSA-SHA2-128s valid verify | available | 184373.688 | 188328.516 | 188518.828 | 188566.406 | 5408.809 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 209520.844 | 215600.253 | 215957.551 | 216046.875 | 4766.186 |
| BIP-340 Schnorr individual valid verify | available | 20613.974 | 21014.079 | 21903.537 | 22177.234 | 48313.360 |
| BIP-340 Schnorr individual invalid verify | available | 20527.979 | 20928.391 | 21747.854 | 22005.141 | 48622.925 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 14151.625 | 14227.581 | 14232.199 | 14233.353 | 70650.358 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 188045.594 | 191420.703 | 194065.391 | 194726.562 | 5320.095 |
| middle_bitflip | available | 185858.062 | 190269.019 | 192742.329 | 193360.656 | 5407.062 |
| last_bitflip | available | 184513.000 | 188596.350 | 189814.070 | 190118.500 | 5434.490 |
| spread_32_bitflips | available | 185053.375 | 188141.928 | 188491.911 | 188579.406 | 5417.429 |
| all_zero | available | 184760.406 | 188818.362 | 189952.223 | 190235.688 | 5406.443 |
| all_ff | available | 186736.969 | 190618.106 | 190748.621 | 190781.250 | 5354.070 |
| deterministic_garbage | available | 195237.000 | 199840.100 | 200419.070 | 200563.812 | 5132.562 |
| wrong_message | available | 209520.844 | 215600.253 | 215957.551 | 216046.875 | 4766.186 |
| wrong_public_key | available | 189738.281 | 196429.441 | 198934.863 | 199561.219 | 5258.053 |
| min observed invalid fixed-length | available | 185053.375 | 188141.928 | 188491.911 | 188579.406 | 5417.429 |
| best observed invalid fixed-length | available | 185053.375 | 188141.928 | 188491.911 | 188579.406 | 5417.429 |
| median observed invalid fixed-length | available | 185858.062 | 190269.019 | 192742.329 | 193360.656 | 5407.062 |
| p99 observed invalid fixed-length | available | 209520.844 | 215600.253 | 215957.551 | 216046.875 | 4766.186 |
| worst observed invalid fixed-length | available | 209520.844 | 215600.253 | 215957.551 | 216046.875 | 4766.186 |

Worst observed invalid fixed-length case: `wrong_message`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 357.526 | 374.208 | 375.208 | 375.458 | 2787445.568 |
| qrs_msg construction | available | 737.365 | 778.474 | 805.686 | 812.490 | 1348668.200 |
| verifier valid call | available | 190772.125 | 195993.888 | 196395.652 | 196496.094 | 5240.463 |
| total modeled valid path | available | 191276.062 | 195683.062 | 197069.938 | 197416.656 | 5238.469 |
| invalid structural reject | available | 2.771 | 2.778 | 2.781 | 2.781 | 360945677.676 |
| invalid fixed-length crypto reject | available | 193792.969 | 198819.812 | 199637.938 | 199842.469 | 5155.578 |

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
| 64 | available | 1384166.667 | 1410097.217 | 21627.604 | 22032.769 |
| 128 | available | 2582736.000 | 2678061.100 | 20177.625 | 20922.352 |
| 512 | available | 8853312.500 | 9278616.033 | 17291.626 | 18122.297 |
| 2048 | available | 32355944.333 | 36497743.067 | 15798.801 | 17821.164 |
| 17391 | available | 246110917.000 | 247512170.800 | 14151.625 | 14232.199 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1392743.000 | 1416477.133 | 21761.609 | 22132.455 |
| 128 | available | 2558284.667 | 2613027.883 | 19986.599 | 20414.280 |
| 512 | available | 8882750.000 | 8953433.433 | 17349.121 | 17487.175 |
| 2048 | available | 31882875.000 | 32610248.633 | 15567.810 | 15922.973 |
| 17391 | available | 241164042.000 | 242324608.100 | 13867.175 | 13933.909 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs. QRS depth rows include the control-block witness weight for the Merkle path and report the resulting TapBranch hash-operation count; timing estimates still use the measured QRS validation-path timing multiplied by the depth-specific weight-limited input count.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 91.449 | 93.411 | 93.505 | 93.529 |
| QRS invalid fixed-length saturated block | available | 496 | 103.922 | 106.938 | 107.115 | 107.159 |
| Schnorr individual saturated block | available | 17391 | 358.498 | 365.456 | 380.924 | 385.684 |
| Schnorr experimental batch saturated block | available | 17391 | 246.111 | 247.432 | 247.512 | 247.532 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS depth-sensitive saturated block estimates

| Model | QRS Merkle depth | Status | Input WU | Max inputs | TapBranch hashes/block | Median ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | 0 | available | 8059 | 496 | 0 | 91.449 | 93.505 | 93.529 |
| QRS valid saturated block | 1 | available | 8091 | 494 | 494 | 91.081 | 93.128 | 93.152 |
| QRS valid saturated block | 3 | available | 8155 | 490 | 1470 | 90.343 | 92.374 | 92.398 |
| QRS valid saturated block | 128 | available | 12155 | 329 | 42112 | 60.659 | 62.023 | 62.038 |
| QRS invalid fixed-length saturated block | 0 | available | 8059 | 496 | 0 | 103.922 | 107.115 | 107.159 |
| QRS invalid fixed-length saturated block | 1 | available | 8091 | 494 | 494 | 103.503 | 106.683 | 106.727 |
| QRS invalid fixed-length saturated block | 3 | available | 8155 | 490 | 1470 | 102.665 | 105.819 | 105.863 |
| QRS invalid fixed-length saturated block | 128 | available | 12155 | 329 | 42112 | 68.932 | 71.050 | 71.079 |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 93.505
- `experimental_batch_schnorr_p99_ms`: 247.512

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
