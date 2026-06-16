# QRS Native Benchmark Report

Mode: `quick`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: a76bb1c988678130fadfa041f68d1e5355e2491e
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
- `git_commit`: a76bb1c988678130fadfa041f68d1e5355e2491e
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
| SLH-DSA-SHA2-128s valid verify | available | 194000.000 | 199358.388 | 200125.878 | 200317.750 | 5154.975 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 199406.250 | 211116.625 | 214131.625 | 214885.375 | 4962.285 |
| BIP-340 Schnorr individual valid verify | available | 20499.709 | 20726.729 | 20792.613 | 20809.084 | 48681.380 |
| BIP-340 Schnorr individual invalid verify | available | 20624.895 | 20923.312 | 20969.846 | 20981.479 | 48419.569 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 14066.097 | 14136.582 | 14139.585 | 14140.336 | 71375.296 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 186526.125 | 194164.050 | 195770.310 | 196171.875 | 5351.746 |
| middle_bitflip | available | 186119.750 | 195667.200 | 197383.440 | 197812.500 | 5325.683 |
| last_bitflip | available | 197713.625 | 199360.475 | 199617.995 | 199682.375 | 5106.984 |
| spread_32_bitflips | available | 191885.375 | 198843.200 | 199089.440 | 199151.000 | 5178.772 |
| all_zero | available | 186656.250 | 196583.900 | 198200.180 | 198604.250 | 5308.266 |
| all_ff | available | 178916.750 | 180268.762 | 180353.753 | 180375.000 | 5626.750 |
| deterministic_garbage | available | 190885.500 | 193704.212 | 193936.742 | 193994.875 | 5221.222 |
| wrong_message | available | 192974.000 | 198887.050 | 200148.310 | 200463.625 | 5201.016 |
| wrong_public_key | available | 199406.250 | 211116.625 | 214131.625 | 214885.375 | 4962.285 |
| min observed invalid fixed-length | available | 178916.750 | 180268.762 | 180353.753 | 180375.000 | 5626.750 |
| best observed invalid fixed-length | available | 178916.750 | 180268.762 | 180353.753 | 180375.000 | 5626.750 |
| median observed invalid fixed-length | available | 186656.250 | 196583.900 | 198200.180 | 198604.250 | 5308.266 |
| p99 observed invalid fixed-length | available | 199406.250 | 211116.625 | 214131.625 | 214885.375 | 4962.285 |
| worst observed invalid fixed-length | available | 199406.250 | 211116.625 | 214131.625 | 214885.375 | 4962.285 |

Worst observed invalid fixed-length case: `wrong_public_key`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields using explicit non-consensus prefixes QRS modeled TapLeaf v0, QRS modeled Branch v0, and QRS modeled SigMsg v0; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 376.625 | 452.440 | 475.105 | 480.771 | 2550517.090 |
| qrs_msg construction | available | 744.375 | 751.830 | 752.550 | 752.730 | 1343317.380 |
| verifier valid call | available | 199557.250 | 215503.600 | 218529.820 | 219286.375 | 4930.789 |
| total modeled valid path | available | 204093.750 | 223768.725 | 229591.245 | 231046.875 | 4805.182 |
| invalid structural reject | available | 2.750 | 2.771 | 2.771 | 2.771 | 362854106.731 |
| invalid fixed-length crypto reject | available | 194833.375 | 210147.325 | 214983.565 | 216192.625 | 5072.694 |

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
| 64 | available | 1467180.667 | 1587195.560 | 22924.698 | 24799.931 |
| 128 | available | 2593778.000 | 2671496.120 | 20263.891 | 20871.063 |
| 512 | available | 9234653.000 | 9556559.787 | 18036.432 | 18665.156 |
| 2048 | available | 32366416.000 | 32517752.000 | 15803.914 | 15877.809 |
| 17391 | available | 244623500.000 | 245901525.640 | 14066.097 | 14139.585 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1390264.000 | 1425735.453 | 21722.875 | 22277.116 |
| 128 | available | 2587847.333 | 2613435.653 | 20217.557 | 20417.466 |
| 512 | available | 8948416.667 | 8987832.773 | 17477.376 | 17554.361 |
| 2048 | available | 32154334.000 | 32629926.640 | 15700.358 | 15932.581 |
| 17391 | available | 244747583.000 | 245089226.360 | 14073.232 | 14092.877 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs. QRS depth rows include the control-block witness weight for the Merkle path and report the resulting TapBranch hash-operation count; timing estimates still use the measured QRS validation-path timing multiplied by the depth-specific weight-limited input count.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 96.224 | 98.882 | 99.262 | 99.358 |
| QRS invalid fixed-length saturated block | available | 496 | 98.906 | 104.714 | 106.209 | 106.583 |
| Schnorr individual saturated block | available | 17391 | 356.510 | 360.459 | 361.604 | 361.891 |
| Schnorr experimental batch saturated block | available | 17391 | 244.624 | 245.849 | 245.902 | 245.915 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS depth-sensitive saturated block estimates

| Model | QRS Merkle depth | Status | Input WU | Max inputs | TapBranch hashes/block | Median ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | 0 | available | 8059 | 496 | 0 | 96.224 | 99.262 | 99.358 |
| QRS valid saturated block | 1 | available | 8091 | 494 | 494 | 95.836 | 98.862 | 98.957 |
| QRS valid saturated block | 3 | available | 8155 | 490 | 1470 | 95.060 | 98.062 | 98.156 |
| QRS valid saturated block | 128 | available | 12155 | 329 | 42112 | 63.826 | 65.841 | 65.905 |
| QRS invalid fixed-length saturated block | 0 | available | 8059 | 496 | 0 | 98.906 | 106.209 | 106.583 |
| QRS invalid fixed-length saturated block | 1 | available | 8091 | 494 | 494 | 98.507 | 105.781 | 106.153 |
| QRS invalid fixed-length saturated block | 3 | available | 8155 | 490 | 1470 | 97.709 | 104.924 | 105.294 |
| QRS invalid fixed-length saturated block | 128 | available | 12155 | 329 | 42112 | 65.605 | 70.449 | 70.697 |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 99.262
- `experimental_batch_schnorr_p99_ms`: 245.902

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
