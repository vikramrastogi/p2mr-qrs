# QRS Native Benchmark Report

Mode: `quick`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: be161096e4277a2f20e986d9d700cc4561b9721d
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
- `git_commit`: be161096e4277a2f20e986d9d700cc4561b9721d
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

## SLH-DSA Backend Probe

| Probe | Result |
|---|---|
| keygen | passed |
| sign | passed |
| verify valid | passed |
| verify mutated signature | passed |
| public key bytes | 32 |
| signature bytes | 7856 |
| context string | explicit_empty_context_set |

## Raw Timing Table

Timing statistics are computed as the median of per-batch means, p95 of per-batch means, p99 of per-batch means, and max of per-batch means; they are not individual operation latency samples.

| Benchmark | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| SLH-DSA-SHA2-128s valid verify | available | 195697.875 | 211335.375 | 214312.875 | 215057.250 | 5084.843 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 195958.375 | 213810.387 | 216745.378 | 217479.125 | 5026.535 |
| BIP-340 Schnorr individual valid verify | available | 19854.208 | 20597.031 | 20611.589 | 20615.229 | 50110.000 |
| BIP-340 Schnorr individual invalid verify | available | 20027.771 | 20231.448 | 20258.956 | 20265.834 | 49929.649 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13628.016 | 13982.019 | 14016.293 | 14024.862 | 72949.853 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 195958.375 | 213810.387 | 216745.378 | 217479.125 | 5026.535 |
| middle_bitflip | available | 181791.750 | 193885.938 | 195052.188 | 195343.750 | 5405.927 |
| last_bitflip | available | 197177.125 | 205918.763 | 206221.252 | 206296.875 | 5027.268 |
| spread_32_bitflips | available | 169026.000 | 187493.200 | 192006.940 | 193135.375 | 5776.149 |
| all_zero | available | 195708.375 | 199370.325 | 199549.065 | 199593.750 | 5115.557 |
| all_ff | available | 195645.875 | 199564.062 | 199762.812 | 199812.500 | 5116.803 |
| deterministic_garbage | available | 188421.875 | 191933.337 | 192690.867 | 192880.250 | 5438.301 |
| wrong_message | available | 171328.125 | 190910.450 | 193240.490 | 193823.000 | 5669.211 |
| wrong_public_key | available | 187322.875 | 196546.862 | 199146.872 | 199796.875 | 5352.514 |
| min observed invalid fixed-length | available | 169026.000 | 187493.200 | 192006.940 | 193135.375 | 5776.149 |
| best observed invalid fixed-length | available | 169026.000 | 187493.200 | 192006.940 | 193135.375 | 5776.149 |
| median observed invalid fixed-length | available | 187322.875 | 196546.862 | 199146.872 | 199796.875 | 5352.514 |
| p99 observed invalid fixed-length | available | 195958.375 | 213810.387 | 216745.378 | 217479.125 | 5026.535 |
| worst observed invalid fixed-length | available | 195958.375 | 213810.387 | 216745.378 | 217479.125 | 5026.535 |

Worst observed invalid fixed-length case: `byte0_bitflip`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields using explicit non-consensus prefixes QRS modeled TapLeaf v0, QRS modeled Branch v0, and QRS modeled SigMsg v0; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 377.833 | 393.025 | 395.305 | 395.875 | 2676118.718 |
| qrs_msg construction | available | 743.167 | 782.331 | 786.766 | 787.875 | 1337755.315 |
| verifier valid call | available | 177364.500 | 187477.038 | 187874.508 | 187973.875 | 5545.950 |
| total modeled valid path | available | 193630.125 | 196736.488 | 196801.497 | 196817.750 | 5221.506 |
| invalid structural reject | available | 2.854 | 2.875 | 2.875 | 2.875 | 350745333.834 |
| invalid fixed-length crypto reject | available | 180338.500 | 196251.025 | 197846.005 | 198244.750 | 5387.788 |

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
| 64 | available | 1351236.000 | 1366846.213 | 21113.062 | 21356.972 |
| 128 | available | 2512430.333 | 2570369.653 | 19628.362 | 20081.013 |
| 512 | available | 8687041.667 | 8838994.440 | 16966.878 | 17263.661 |
| 2048 | available | 31204292.000 | 31293733.360 | 15236.471 | 15280.143 |
| 17391 | available | 237004833.000 | 243757358.320 | 13628.016 | 14016.293 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1335319.667 | 1370520.347 | 20864.370 | 21414.380 |
| 128 | available | 2477347.333 | 2529496.773 | 19354.276 | 19761.694 |
| 512 | available | 8697708.333 | 8755148.880 | 16987.712 | 17099.900 |
| 2048 | available | 31085250.000 | 31293451.640 | 15178.345 | 15280.006 |
| 17391 | available | 238195208.000 | 238550778.000 | 13696.464 | 13716.910 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs. QRS depth rows include the control-block witness weight for the Merkle path and report the resulting TapBranch hash-operation count; timing estimates still use the measured QRS validation-path timing multiplied by the depth-specific weight-limited input count.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 97.066 | 104.822 | 106.299 | 106.668 |
| QRS invalid fixed-length saturated block | available | 496 | 97.195 | 106.050 | 107.506 | 107.870 |
| Schnorr individual saturated block | available | 17391 | 345.285 | 358.203 | 358.456 | 358.519 |
| Schnorr experimental batch saturated block | available | 17391 | 237.005 | 243.161 | 243.757 | 243.906 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS depth-sensitive saturated block estimates

| Model | QRS Merkle depth | Status | Input WU | Max inputs | TapBranch hashes/block | Median ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | 0 | available | 8059 | 496 | 0 | 97.066 | 106.299 | 106.668 |
| QRS valid saturated block | 1 | available | 8091 | 494 | 494 | 96.675 | 105.871 | 106.238 |
| QRS valid saturated block | 3 | available | 8155 | 490 | 1470 | 95.892 | 105.013 | 105.378 |
| QRS valid saturated block | 128 | available | 12155 | 329 | 42112 | 64.385 | 70.509 | 70.754 |
| QRS invalid fixed-length saturated block | 0 | available | 8059 | 496 | 0 | 97.195 | 107.506 | 107.870 |
| QRS invalid fixed-length saturated block | 1 | available | 8091 | 494 | 494 | 96.803 | 107.072 | 107.435 |
| QRS invalid fixed-length saturated block | 3 | available | 8155 | 490 | 1470 | 96.020 | 106.205 | 106.565 |
| QRS invalid fixed-length saturated block | 128 | available | 12155 | 329 | 42112 | 64.470 | 71.309 | 71.551 |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 106.299
- `experimental_batch_schnorr_p99_ms`: 243.757

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
