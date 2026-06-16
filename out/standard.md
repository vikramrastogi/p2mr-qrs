# QRS Native Benchmark Report

Mode: `standard`

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
| SLH-DSA-SHA2-128s valid verify | available | 169610.688 | 185527.850 | 192599.320 | 194367.188 | 5809.884 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 192324.219 | 205079.044 | 209453.309 | 210546.875 | 5192.398 |
| BIP-340 Schnorr individual valid verify | available | 20041.224 | 20460.911 | 20508.377 | 20523.031 | 49806.664 |
| BIP-340 Schnorr individual invalid verify | available | 19913.635 | 20600.733 | 20677.141 | 20678.427 | 49961.822 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13712.950 | 14129.763 | 14154.830 | 14161.096 | 72641.316 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 172802.094 | 189964.984 | 196394.047 | 198001.312 | 5728.160 |
| middle_bitflip | available | 183481.750 | 191939.569 | 200691.039 | 202878.906 | 5418.879 |
| last_bitflip | available | 171884.125 | 177730.469 | 178058.594 | 178140.625 | 5822.198 |
| spread_32_bitflips | available | 200329.438 | 204786.072 | 206856.164 | 207373.688 | 5000.886 |
| all_zero | available | 188028.656 | 198708.069 | 202402.039 | 203325.531 | 5275.059 |
| all_ff | available | 182950.531 | 187980.581 | 190545.066 | 191186.188 | 5457.641 |
| deterministic_garbage | available | 178328.125 | 188849.084 | 190698.992 | 191161.469 | 5543.696 |
| wrong_message | available | 192324.219 | 205079.044 | 209453.309 | 210546.875 | 5192.398 |
| wrong_public_key | available | 175136.719 | 180200.247 | 181072.324 | 181290.344 | 5675.690 |
| min observed invalid fixed-length | available | 171884.125 | 177730.469 | 178058.594 | 178140.625 | 5822.198 |
| best observed invalid fixed-length | available | 171884.125 | 177730.469 | 178058.594 | 178140.625 | 5822.198 |
| median observed invalid fixed-length | available | 172802.094 | 189964.984 | 196394.047 | 198001.312 | 5728.160 |
| p99 observed invalid fixed-length | available | 192324.219 | 205079.044 | 209453.309 | 210546.875 | 5192.398 |
| worst observed invalid fixed-length | available | 192324.219 | 205079.044 | 209453.309 | 210546.875 | 5192.398 |

Worst observed invalid fixed-length case: `wrong_message`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields using explicit non-consensus prefixes QRS modeled TapLeaf v0, QRS modeled Branch v0, and QRS modeled SigMsg v0; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 363.609 | 384.317 | 386.484 | 387.026 | 2747928.972 |
| qrs_msg construction | available | 724.250 | 752.719 | 758.873 | 760.412 | 1372900.179 |
| verifier valid call | available | 194164.062 | 200235.938 | 201172.188 | 201406.250 | 5134.156 |
| total modeled valid path | available | 198631.500 | 204461.059 | 205275.562 | 205479.188 | 5045.532 |
| invalid structural reject | available | 2.838 | 2.863 | 2.889 | 2.896 | 354592115.644 |
| invalid fixed-length crypto reject | available | 183559.906 | 188396.594 | 188820.969 | 188927.062 | 5463.478 |

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
| 64 | available | 1372868.000 | 1401394.400 | 21451.062 | 21896.787 |
| 128 | available | 2531583.333 | 2562654.117 | 19777.995 | 20020.735 |
| 512 | available | 8991930.667 | 9185409.633 | 17562.365 | 17940.253 |
| 2048 | available | 31370694.333 | 31899697.433 | 15317.722 | 15576.024 |
| 17391 | available | 238481917.000 | 246166641.700 | 13712.950 | 14154.830 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1342159.667 | 1383002.133 | 20971.245 | 21609.408 |
| 128 | available | 2517840.333 | 2546795.133 | 19670.628 | 19896.837 |
| 512 | available | 8629305.667 | 8758237.500 | 16854.113 | 17105.933 |
| 2048 | available | 31847541.667 | 40394409.700 | 15550.557 | 19723.833 |
| 17391 | available | 237645167.000 | 245606154.200 | 13664.836 | 14122.601 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs. QRS depth rows include the control-block witness weight for the Merkle path and report the resulting TapBranch hash-operation count; timing estimates still use the measured QRS validation-path timing multiplied by the depth-specific weight-limited input count.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 84.127 | 92.022 | 95.529 | 96.406 |
| QRS invalid fixed-length saturated block | available | 496 | 95.393 | 101.719 | 103.889 | 104.431 |
| Schnorr individual saturated block | available | 17391 | 348.537 | 355.836 | 356.661 | 356.916 |
| Schnorr experimental batch saturated block | available | 17391 | 238.482 | 245.731 | 246.167 | 246.276 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS depth-sensitive saturated block estimates

| Model | QRS Merkle depth | Status | Input WU | Max inputs | TapBranch hashes/block | Median ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | 0 | available | 8059 | 496 | 0 | 84.127 | 95.529 | 96.406 |
| QRS valid saturated block | 1 | available | 8091 | 494 | 494 | 83.788 | 95.144 | 96.017 |
| QRS valid saturated block | 3 | available | 8155 | 490 | 1470 | 83.109 | 94.374 | 95.240 |
| QRS valid saturated block | 128 | available | 12155 | 329 | 42112 | 55.802 | 63.365 | 63.947 |
| QRS invalid fixed-length saturated block | 0 | available | 8059 | 496 | 0 | 95.393 | 103.889 | 104.431 |
| QRS invalid fixed-length saturated block | 1 | available | 8091 | 494 | 494 | 95.008 | 103.470 | 104.010 |
| QRS invalid fixed-length saturated block | 3 | available | 8155 | 490 | 1470 | 94.239 | 102.632 | 103.168 |
| QRS invalid fixed-length saturated block | 128 | available | 12155 | 329 | 42112 | 63.275 | 68.910 | 69.270 |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 95.529
- `experimental_batch_schnorr_p99_ms`: 246.167

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
