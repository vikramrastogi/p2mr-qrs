# QRS Native Benchmark Report

Mode: `full`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: 12d98c69248da475cfcf54409c01eee9418cdc69
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
- `git_commit`: 12d98c69248da475cfcf54409c01eee9418cdc69
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
| SLH-DSA-SHA2-128s valid verify | available | 183070.830 | 188046.870 | 189428.829 | 189734.580 | 5462.444 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 184324.580 | 188933.745 | 217210.499 | 228646.250 | 5384.869 |
| BIP-340 Schnorr individual valid verify | available | 20669.352 | 21000.964 | 24964.066 | 28332.150 | 47938.513 |
| BIP-340 Schnorr individual invalid verify | available | 20793.021 | 21061.730 | 21441.235 | 21598.635 | 48172.260 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 14162.613 | 16632.398 | 17522.934 | 17745.568 | 68844.237 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 199458.330 | 203017.295 | 204842.963 | 205622.090 | 5012.092 |
| middle_bitflip | available | 198387.090 | 202013.750 | 204096.706 | 204613.330 | 5037.465 |
| last_bitflip | available | 184724.590 | 188570.415 | 189711.538 | 190145.410 | 5412.169 |
| spread_32_bitflips | available | 191475.000 | 195915.205 | 198381.248 | 199437.500 | 5205.603 |
| all_zero | available | 179568.750 | 183457.920 | 185081.839 | 185452.090 | 5554.680 |
| all_ff | available | 192808.340 | 196868.540 | 198066.045 | 198360.420 | 5172.767 |
| deterministic_garbage | available | 184911.670 | 188509.795 | 188726.669 | 188769.170 | 5411.125 |
| wrong_message | available | 178788.750 | 182137.500 | 182330.705 | 182382.080 | 5596.781 |
| wrong_public_key | available | 184324.580 | 188933.745 | 217210.499 | 228646.250 | 5384.869 |
| min observed invalid fixed-length | available | 178788.750 | 182137.500 | 182330.705 | 182382.080 | 5596.781 |
| best observed invalid fixed-length | available | 178788.750 | 182137.500 | 182330.705 | 182382.080 | 5596.781 |
| median observed invalid fixed-length | available | 192808.340 | 196868.540 | 198066.045 | 198360.420 | 5172.767 |
| p99 observed invalid fixed-length | available | 184324.580 | 188933.745 | 217210.499 | 228646.250 | 5384.869 |
| worst observed invalid fixed-length | available | 184324.580 | 188933.745 | 217210.499 | 228646.250 | 5384.869 |

Worst observed invalid fixed-length case: `wrong_public_key`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields using explicit non-consensus prefixes QRS modeled TapLeaf v0, QRS modeled Branch v0, and QRS modeled SigMsg v0; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 362.550 | 372.655 | 374.226 | 374.777 | 2749579.802 |
| qrs_msg construction | available | 746.731 | 758.674 | 763.599 | 764.635 | 1337865.536 |
| verifier valid call | available | 184787.920 | 186986.665 | 189789.957 | 190938.330 | 5414.128 |
| total modeled valid path | available | 184744.580 | 186458.125 | 187890.418 | 188474.170 | 5414.552 |
| invalid structural reject | available | 2.715 | 2.719 | 2.726 | 2.729 | 368171021.378 |
| invalid fixed-length crypto reject | available | 181518.750 | 184074.790 | 184894.500 | 185223.750 | 5500.589 |

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
| 64 | available | 1373868.000 | 1384250.717 | 21466.688 | 21628.917 |
| 128 | available | 2635444.417 | 2685096.483 | 20589.410 | 20977.316 |
| 512 | available | 8924024.333 | 9147331.950 | 17429.735 | 17865.883 |
| 2048 | available | 31966833.400 | 33103588.240 | 15608.805 | 16163.861 |
| 17391 | available | 246302000.000 | 304741342.000 | 14162.613 | 17522.934 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1425375.000 | 1468171.550 | 22271.484 | 22940.180 |
| 128 | available | 2598871.583 | 2621487.517 | 20303.684 | 20480.371 |
| 512 | available | 9013961.833 | 9923322.867 | 17605.394 | 19381.490 |
| 2048 | available | 32197075.000 | 33710031.680 | 15721.228 | 16459.976 |
| 17391 | available | 244930270.500 | 250833466.800 | 14083.737 | 14423.177 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs. QRS depth rows include the control-block witness weight for the Merkle path and report the resulting TapBranch hash-operation count; timing estimates still use the measured QRS validation-path timing multiplied by the depth-specific weight-limited input count.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 90.803 | 93.271 | 93.957 | 94.108 |
| QRS invalid fixed-length saturated block | available | 496 | 91.425 | 93.711 | 107.736 | 113.409 |
| Schnorr individual saturated block | available | 17391 | 359.461 | 365.228 | 434.150 | 492.724 |
| Schnorr experimental batch saturated block | available | 17391 | 246.302 | 289.254 | 304.741 | 308.613 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS depth-sensitive saturated block estimates

| Model | QRS Merkle depth | Status | Input WU | Max inputs | TapBranch hashes/block | Median ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | 0 | available | 8059 | 496 | 0 | 90.803 | 93.957 | 94.108 |
| QRS valid saturated block | 1 | available | 8091 | 494 | 494 | 90.437 | 93.578 | 93.729 |
| QRS valid saturated block | 3 | available | 8155 | 490 | 1470 | 89.705 | 92.820 | 92.970 |
| QRS valid saturated block | 128 | available | 12155 | 329 | 42112 | 60.230 | 62.322 | 62.423 |
| QRS invalid fixed-length saturated block | 0 | available | 8059 | 496 | 0 | 91.425 | 107.736 | 113.409 |
| QRS invalid fixed-length saturated block | 1 | available | 8091 | 494 | 494 | 91.056 | 107.302 | 112.951 |
| QRS invalid fixed-length saturated block | 3 | available | 8155 | 490 | 1470 | 90.319 | 106.433 | 112.037 |
| QRS invalid fixed-length saturated block | 128 | available | 12155 | 329 | 42112 | 60.643 | 71.462 | 75.225 |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 93.957
- `experimental_batch_schnorr_p99_ms`: 304.741

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
