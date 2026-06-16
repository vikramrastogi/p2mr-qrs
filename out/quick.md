# QRS Native Benchmark Report

Mode: `quick`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: 315933af97fddec85c88d4547186753369a43e2f
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
- `git_commit`: 315933af97fddec85c88d4547186753369a43e2f
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
| SLH-DSA-SHA2-128s valid verify | available | 190776.000 | 194131.812 | 194630.562 | 194755.250 | 5264.642 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 202135.375 | 206934.375 | 207624.375 | 207796.875 | 4964.411 |
| BIP-340 Schnorr individual valid verify | available | 19948.709 | 20378.083 | 20426.950 | 20439.166 | 50066.628 |
| BIP-340 Schnorr individual invalid verify | available | 20594.500 | 21198.333 | 21467.817 | 21535.188 | 48870.020 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13737.129 | 14103.750 | 14143.499 | 14153.437 | 72297.611 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 196822.875 | 204490.100 | 204531.320 | 204541.625 | 5072.005 |
| middle_bitflip | available | 183510.375 | 193902.138 | 196784.628 | 197505.250 | 5421.759 |
| last_bitflip | available | 192270.875 | 198133.850 | 199022.570 | 199244.750 | 5185.065 |
| spread_32_bitflips | available | 191380.125 | 201867.188 | 202298.438 | 202406.250 | 5184.925 |
| all_zero | available | 174979.125 | 185566.663 | 187884.133 | 188463.500 | 5635.811 |
| all_ff | available | 187140.625 | 187881.725 | 187892.945 | 187895.750 | 5347.913 |
| deterministic_garbage | available | 180885.375 | 193871.425 | 195395.185 | 195776.125 | 5424.779 |
| wrong_message | available | 192333.250 | 196200.000 | 196477.500 | 196546.875 | 5255.092 |
| wrong_public_key | available | 202135.375 | 206934.375 | 207624.375 | 207796.875 | 4964.411 |
| min observed invalid fixed-length | available | 174979.125 | 185566.663 | 187884.133 | 188463.500 | 5635.811 |
| best observed invalid fixed-length | available | 174979.125 | 185566.663 | 187884.133 | 188463.500 | 5635.811 |
| median observed invalid fixed-length | available | 183510.375 | 193902.138 | 196784.628 | 197505.250 | 5421.759 |
| p99 observed invalid fixed-length | available | 202135.375 | 206934.375 | 207624.375 | 207796.875 | 4964.411 |
| worst observed invalid fixed-length | available | 202135.375 | 206934.375 | 207624.375 | 207796.875 | 4964.411 |

Worst observed invalid fixed-length case: `wrong_public_key`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 346.125 | 370.350 | 376.820 | 378.438 | 2852364.855 |
| qrs_msg construction | available | 722.604 | 747.346 | 748.286 | 748.520 | 1384037.481 |
| verifier valid call | available | 185515.625 | 192738.038 | 194289.307 | 194677.125 | 5361.887 |
| total modeled valid path | available | 184880.250 | 200024.550 | 201938.310 | 202416.750 | 5303.469 |
| invalid structural reject | available | 2.875 | 2.889 | 2.894 | 2.896 | 348544825.354 |
| invalid fixed-length crypto reject | available | 185317.625 | 194890.087 | 195836.318 | 196072.875 | 5457.203 |

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
| 64 | available | 1457430.667 | 1556006.880 | 22772.354 | 24312.607 |
| 128 | available | 2719875.000 | 2935906.107 | 21249.023 | 22936.766 |
| 512 | available | 8825014.000 | 8845701.013 | 17236.355 | 17276.760 |
| 2048 | available | 31445042.000 | 31648389.000 | 15354.024 | 15453.315 |
| 17391 | available | 238902416.000 | 245969594.360 | 13737.129 | 14143.499 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1419569.333 | 1476774.440 | 22180.771 | 23074.601 |
| 128 | available | 2591666.667 | 2624700.333 | 20247.396 | 20505.471 |
| 512 | available | 9027833.333 | 9144520.560 | 17632.487 | 17860.392 |
| 2048 | available | 31801167.000 | 31956304.360 | 15527.914 | 15603.664 |
| 17391 | available | 239890458.000 | 264319546.640 | 13793.943 | 15198.640 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 94.625 | 96.289 | 96.537 | 96.599 |
| QRS invalid fixed-length saturated block | available | 496 | 100.259 | 102.639 | 102.982 | 103.067 |
| Schnorr individual saturated block | available | 17391 | 346.928 | 354.395 | 355.245 | 355.458 |
| Schnorr experimental batch saturated block | available | 17391 | 238.902 | 245.278 | 245.970 | 246.142 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 96.537
- `experimental_batch_schnorr_p99_ms`: 245.970

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
