# QRS Native Benchmark Report

Mode: `standard`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: e1777269ac216a8b79a396d0b307c514136df4b3
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
- `git_commit`: e1777269ac216a8b79a396d0b307c514136df4b3
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
| SLH-DSA-SHA2-128s valid verify | available | 203580.750 | 223694.672 | 234293.109 | 236942.719 | 4840.051 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 195479.188 | 228304.803 | 249142.211 | 254351.562 | 4971.329 |
| BIP-340 Schnorr individual valid verify | available | 20098.911 | 20545.219 | 25963.701 | 27667.651 | 48990.285 |
| BIP-340 Schnorr individual invalid verify | available | 20050.427 | 20498.985 | 20514.088 | 20515.312 | 49723.528 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13745.072 | 13946.612 | 13951.711 | 13952.986 | 72509.047 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 192140.625 | 198583.475 | 198843.770 | 198908.844 | 5169.072 |
| middle_bitflip | available | 193798.188 | 200959.372 | 201328.324 | 201420.562 | 5130.946 |
| last_bitflip | available | 198981.781 | 207530.194 | 207671.664 | 207707.031 | 4995.087 |
| spread_32_bitflips | available | 199812.500 | 211184.234 | 214038.922 | 214752.594 | 4963.342 |
| all_zero | available | 178736.969 | 184747.772 | 187840.179 | 188613.281 | 5577.253 |
| all_ff | available | 175250.000 | 180697.797 | 181968.734 | 182286.469 | 5700.873 |
| deterministic_garbage | available | 195479.188 | 228304.803 | 249142.211 | 254351.562 | 4971.329 |
| wrong_message | available | 192811.188 | 213763.281 | 215443.281 | 215863.281 | 5149.266 |
| wrong_public_key | available | 206907.562 | 229329.809 | 232436.812 | 233213.562 | 4819.342 |
| min observed invalid fixed-length | available | 175250.000 | 180697.797 | 181968.734 | 182286.469 | 5700.873 |
| best observed invalid fixed-length | available | 175250.000 | 180697.797 | 181968.734 | 182286.469 | 5700.873 |
| median observed invalid fixed-length | available | 198981.781 | 207530.194 | 207671.664 | 207707.031 | 4995.087 |
| p99 observed invalid fixed-length | available | 195479.188 | 228304.803 | 249142.211 | 254351.562 | 4971.329 |
| worst observed invalid fixed-length | available | 195479.188 | 228304.803 | 249142.211 | 254351.562 | 4971.329 |

Worst observed invalid fixed-length case: `deterministic_garbage`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 373.682 | 387.439 | 390.571 | 391.354 | 2690967.599 |
| qrs_msg construction | available | 746.505 | 764.912 | 771.524 | 773.177 | 1338207.248 |
| verifier valid call | available | 195701.844 | 201151.166 | 202993.758 | 203454.406 | 5095.907 |
| total modeled valid path | available | 198371.094 | 199521.353 | 199950.121 | 200057.312 | 5082.962 |
| invalid structural reject | available | 2.651 | 2.678 | 2.690 | 2.693 | 376563917.018 |
| invalid fixed-length crypto reject | available | 188524.719 | 194952.987 | 196165.598 | 196468.750 | 5279.393 |

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
| 64 | available | 1359041.667 | 1409702.833 | 21235.026 | 22026.607 |
| 128 | available | 2524861.167 | 2565720.183 | 19725.478 | 20044.689 |
| 512 | available | 8795027.833 | 8945161.867 | 17177.789 | 17471.019 |
| 2048 | available | 31775847.333 | 32019727.967 | 15515.550 | 15634.633 |
| 17391 | available | 239040541.000 | 242634204.200 | 13745.072 | 13951.711 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1358513.833 | 1413831.933 | 21226.779 | 22091.124 |
| 128 | available | 2557131.833 | 2598395.083 | 19977.592 | 20299.962 |
| 512 | available | 8760159.667 | 8788131.933 | 17109.687 | 17164.320 |
| 2048 | available | 31883430.667 | 32491983.433 | 15568.081 | 15865.226 |
| 17391 | available | 241461167.000 | 245738862.800 | 13884.260 | 14130.232 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 100.976 | 110.953 | 116.209 | 117.524 |
| QRS invalid fixed-length saturated block | available | 496 | 96.958 | 113.239 | 123.575 | 126.158 |
| Schnorr individual saturated block | available | 17391 | 349.540 | 357.302 | 451.535 | 481.168 |
| Schnorr experimental batch saturated block | available | 17391 | 239.041 | 242.546 | 242.634 | 242.656 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 116.209
- `experimental_batch_schnorr_p99_ms`: 242.634

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
