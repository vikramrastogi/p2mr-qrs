# QRS Native Benchmark Report

Mode: `standard`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: 51f6d6fc8885d7a81fde84810cae8f372828775e
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
- `git_commit`: 51f6d6fc8885d7a81fde84810cae8f372828775e
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
| SLH-DSA-SHA2-128s valid verify | available | 193313.812 | 198380.325 | 198589.590 | 198641.906 | 5189.928 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 188755.188 | 195863.281 | 202753.906 | 204476.562 | 5279.091 |
| BIP-340 Schnorr individual valid verify | available | 20274.839 | 20632.943 | 21159.634 | 21325.328 | 49160.824 |
| BIP-340 Schnorr individual invalid verify | available | 20303.469 | 20614.565 | 20738.437 | 20775.948 | 49133.405 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13942.657 | 14056.825 | 14060.981 | 14062.020 | 71784.829 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 191561.188 | 196561.987 | 196913.422 | 197001.281 | 5222.010 |
| middle_bitflip | available | 196838.531 | 200756.775 | 201262.805 | 201389.312 | 5083.615 |
| last_bitflip | available | 193126.281 | 196847.409 | 197777.832 | 198010.438 | 5162.398 |
| spread_32_bitflips | available | 188417.969 | 193391.406 | 194297.031 | 194523.438 | 5296.109 |
| all_zero | available | 181368.469 | 186498.550 | 187973.660 | 188342.438 | 5514.996 |
| all_ff | available | 170423.156 | 175156.387 | 176875.028 | 177304.688 | 5867.406 |
| deterministic_garbage | available | 183860.688 | 187360.153 | 188936.606 | 189330.719 | 5431.674 |
| wrong_message | available | 192109.375 | 196107.672 | 198367.359 | 198932.281 | 5197.217 |
| wrong_public_key | available | 188755.188 | 195863.281 | 202753.906 | 204476.562 | 5279.091 |
| min observed invalid fixed-length | available | 170423.156 | 175156.387 | 176875.028 | 177304.688 | 5867.406 |
| best observed invalid fixed-length | available | 170423.156 | 175156.387 | 176875.028 | 177304.688 | 5867.406 |
| median observed invalid fixed-length | available | 191561.188 | 196561.987 | 196913.422 | 197001.281 | 5222.010 |
| p99 observed invalid fixed-length | available | 188755.188 | 195863.281 | 202753.906 | 204476.562 | 5279.091 |
| worst observed invalid fixed-length | available | 188755.188 | 195863.281 | 202753.906 | 204476.562 | 5279.091 |

Worst observed invalid fixed-length case: `wrong_public_key`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 373.786 | 384.614 | 392.002 | 393.849 | 2694066.279 |
| qrs_msg construction | available | 742.240 | 754.468 | 754.748 | 754.818 | 1351851.841 |
| verifier valid call | available | 195506.500 | 203769.788 | 206514.383 | 207200.531 | 5099.681 |
| total modeled valid path | available | 194365.875 | 197401.562 | 198880.312 | 199250.000 | 5159.346 |
| invalid structural reject | available | 2.729 | 3.208 | 3.296 | 3.318 | 357581319.952 |
| invalid fixed-length crypto reject | available | 181964.844 | 187784.372 | 189233.949 | 189596.344 | 5496.886 |

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
| 64 | available | 1399090.333 | 1437625.000 | 21860.786 | 22462.891 |
| 128 | available | 2608979.167 | 2655225.650 | 20382.650 | 20743.950 |
| 512 | available | 8852889.000 | 9147544.550 | 17290.799 | 17866.298 |
| 2048 | available | 31899958.333 | 32394531.933 | 15576.152 | 15817.643 |
| 17391 | available | 242476750.000 | 244534516.400 | 13942.657 | 14060.981 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1399972.167 | 1437497.183 | 21874.565 | 22460.893 |
| 128 | available | 2581027.667 | 2636318.000 | 20164.279 | 20596.234 |
| 512 | available | 8831826.333 | 8935662.450 | 17249.661 | 17452.466 |
| 2048 | available | 32377597.000 | 33262288.800 | 15809.374 | 16241.352 |
| 17391 | available | 246463333.000 | 271780295.800 | 14171.890 | 15627.640 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 95.884 | 98.397 | 98.500 | 98.526 |
| QRS invalid fixed-length saturated block | available | 496 | 93.623 | 97.148 | 100.566 | 101.420 |
| Schnorr individual saturated block | available | 17391 | 352.600 | 358.828 | 367.987 | 370.869 |
| Schnorr experimental batch saturated block | available | 17391 | 242.477 | 244.462 | 244.535 | 244.553 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 98.500
- `experimental_batch_schnorr_p99_ms`: 244.535

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
