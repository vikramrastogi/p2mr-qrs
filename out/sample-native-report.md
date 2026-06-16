# QRS Native Benchmark Report

Mode: `quick`

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
| SLH-DSA-SHA2-128s valid verify | available | 180281.250 | 182416.650 | 182616.630 | 182666.625 | 5541.811 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 191302.000 | 204906.812 | 206973.062 | 207489.625 | 5144.577 |
| BIP-340 Schnorr individual valid verify | available | 19861.292 | 20142.646 | 20219.312 | 20238.479 | 50187.376 |
| BIP-340 Schnorr individual invalid verify | available | 19653.229 | 20125.240 | 20225.882 | 20251.042 | 50689.507 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13536.942 | 13552.558 | 13552.773 | 13552.827 | 73907.465 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 191302.000 | 204906.812 | 206973.062 | 207489.625 | 5144.577 |
| middle_bitflip | available | 180354.125 | 185637.038 | 185710.807 | 185729.250 | 5563.650 |
| last_bitflip | available | 177125.000 | 178883.837 | 178922.568 | 178932.250 | 5643.431 |
| spread_32_bitflips | available | 174614.625 | 187127.112 | 189892.122 | 190583.375 | 5647.604 |
| all_zero | available | 174197.875 | 188894.213 | 188970.443 | 188989.500 | 5597.878 |
| all_ff | available | 181635.375 | 183355.237 | 183675.247 | 183755.250 | 5501.140 |
| deterministic_garbage | available | 177229.250 | 187106.237 | 189983.747 | 190703.125 | 5632.788 |
| wrong_message | available | 174166.750 | 189350.550 | 190799.310 | 191161.500 | 5625.667 |
| wrong_public_key | available | 182354.125 | 190326.537 | 191465.307 | 191750.000 | 5427.321 |
| min observed invalid fixed-length | available | 177125.000 | 178883.837 | 178922.568 | 178932.250 | 5643.431 |
| best observed invalid fixed-length | available | 177125.000 | 178883.837 | 178922.568 | 178932.250 | 5643.431 |
| median observed invalid fixed-length | available | 174614.625 | 187127.112 | 189892.122 | 190583.375 | 5647.604 |
| p99 observed invalid fixed-length | available | 191302.000 | 204906.812 | 206973.062 | 207489.625 | 5144.577 |
| worst observed invalid fixed-length | available | 191302.000 | 204906.812 | 206973.062 | 207489.625 | 5144.577 |

Worst observed invalid fixed-length case: `byte0_bitflip`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields using explicit non-consensus prefixes QRS modeled TapLeaf v0, QRS modeled Branch v0, and QRS modeled SigMsg v0; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 343.250 | 346.573 | 347.298 | 347.479 | 2908386.249 |
| qrs_msg construction | available | 701.562 | 749.952 | 754.957 | 756.208 | 1398729.674 |
| verifier valid call | available | 185401.125 | 195694.312 | 197730.562 | 198239.625 | 5367.626 |
| total modeled valid path | available | 182578.125 | 190795.775 | 192375.755 | 192770.750 | 5423.488 |
| invalid structural reject | available | 2.646 | 2.660 | 2.665 | 2.667 | 377969762.419 |
| invalid fixed-length crypto reject | available | 177635.500 | 188705.712 | 190349.443 | 190760.375 | 5540.965 |

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
| 64 | available | 1319625.000 | 1383806.560 | 20619.141 | 21621.978 |
| 128 | available | 2562902.667 | 2616725.000 | 20022.677 | 20443.164 |
| 512 | available | 8609625.000 | 8688335.560 | 16815.674 | 16969.405 |
| 2048 | available | 32205083.000 | 32367503.000 | 15725.138 | 15804.445 |
| 17391 | available | 235420959.000 | 235696273.040 | 13536.942 | 13552.773 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1336791.667 | 1363106.560 | 20887.370 | 21298.540 |
| 128 | available | 2592680.667 | 2698110.893 | 20255.318 | 21078.991 |
| 512 | available | 8936944.333 | 9047172.227 | 17454.969 | 17670.258 |
| 2048 | available | 32284875.000 | 32410556.960 | 15764.099 | 15825.467 |
| 17391 | available | 235891583.000 | 236030938.000 | 13564.003 | 13572.016 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs. QRS depth rows include the control-block witness weight for the Merkle path and report the resulting TapBranch hash-operation count; timing estimates still use the measured QRS validation-path timing multiplied by the depth-specific weight-limited input count.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 89.419 | 90.479 | 90.578 | 90.603 |
| QRS invalid fixed-length saturated block | available | 496 | 94.886 | 101.634 | 102.659 | 102.915 |
| Schnorr individual saturated block | available | 17391 | 345.408 | 350.301 | 351.634 | 351.967 |
| Schnorr experimental batch saturated block | available | 17391 | 235.421 | 235.693 | 235.696 | 235.697 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS depth-sensitive saturated block estimates

| Model | QRS Merkle depth | Status | Input WU | Max inputs | TapBranch hashes/block | Median ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | 0 | available | 8059 | 496 | 0 | 89.419 | 90.578 | 90.603 |
| QRS valid saturated block | 1 | available | 8091 | 494 | 494 | 89.059 | 90.213 | 90.237 |
| QRS valid saturated block | 3 | available | 8155 | 490 | 1470 | 88.338 | 89.482 | 89.507 |
| QRS valid saturated block | 128 | available | 12155 | 329 | 42112 | 59.313 | 60.081 | 60.097 |
| QRS invalid fixed-length saturated block | 0 | available | 8059 | 496 | 0 | 94.886 | 102.659 | 102.915 |
| QRS invalid fixed-length saturated block | 1 | available | 8091 | 494 | 494 | 94.503 | 102.245 | 102.500 |
| QRS invalid fixed-length saturated block | 3 | available | 8155 | 490 | 1470 | 93.738 | 101.417 | 101.670 |
| QRS invalid fixed-length saturated block | 128 | available | 12155 | 329 | 42112 | 62.938 | 68.094 | 68.264 |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 90.578
- `experimental_batch_schnorr_p99_ms`: 235.696

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
