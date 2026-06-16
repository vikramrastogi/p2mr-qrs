# QRS Native Benchmark Report

Mode: `full`

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
| SLH-DSA-SHA2-128s valid verify | available | 188460.420 | 195628.540 | 198000.375 | 198168.750 | 5296.374 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 191707.500 | 198617.915 | 203841.123 | 205811.250 | 5190.981 |
| BIP-340 Schnorr individual valid verify | available | 19939.946 | 20246.864 | 20377.136 | 20480.702 | 50067.401 |
| BIP-340 Schnorr individual invalid verify | available | 19807.498 | 20100.355 | 20923.720 | 21703.815 | 50290.207 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13555.339 | 13789.290 | 13871.990 | 13892.665 | 73580.080 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 188247.080 | 194488.955 | 196068.749 | 196287.500 | 5317.038 |
| middle_bitflip | available | 193966.250 | 197473.125 | 198766.751 | 199047.500 | 5148.341 |
| last_bitflip | available | 191707.500 | 198617.915 | 203841.123 | 205811.250 | 5190.981 |
| spread_32_bitflips | available | 186503.330 | 194317.915 | 194587.912 | 194607.910 | 5345.372 |
| all_zero | available | 177505.420 | 180262.710 | 180644.670 | 180784.170 | 5639.961 |
| all_ff | available | 176946.670 | 183464.165 | 184677.418 | 184821.670 | 5644.967 |
| deterministic_garbage | available | 181862.920 | 186193.960 | 189156.170 | 190329.170 | 5481.697 |
| wrong_message | available | 181020.420 | 184048.540 | 184634.455 | 184813.330 | 5506.713 |
| wrong_public_key | available | 179701.250 | 185850.205 | 186894.337 | 187105.840 | 5561.376 |
| min observed invalid fixed-length | available | 177505.420 | 180262.710 | 180644.670 | 180784.170 | 5639.961 |
| best observed invalid fixed-length | available | 177505.420 | 180262.710 | 180644.670 | 180784.170 | 5639.961 |
| median observed invalid fixed-length | available | 181862.920 | 186193.960 | 189156.170 | 190329.170 | 5481.697 |
| p99 observed invalid fixed-length | available | 191707.500 | 198617.915 | 203841.123 | 205811.250 | 5190.981 |
| worst observed invalid fixed-length | available | 191707.500 | 198617.915 | 203841.123 | 205811.250 | 5190.981 |

Worst observed invalid fixed-length case: `last_bitflip`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields using explicit non-consensus prefixes QRS modeled TapLeaf v0, QRS modeled Branch v0, and QRS modeled SigMsg v0; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 353.160 | 364.186 | 365.734 | 366.054 | 2812465.827 |
| qrs_msg construction | available | 726.298 | 730.864 | 731.050 | 731.129 | 1378037.195 |
| verifier valid call | available | 183337.910 | 187583.335 | 189511.375 | 189782.500 | 5438.449 |
| total modeled valid path | available | 184495.000 | 185850.415 | 186333.500 | 186526.250 | 5419.226 |
| invalid structural reject | available | 2.844 | 3.545 | 4.390 | 4.748 | 330652029.803 |
| invalid fixed-length crypto reject | available | 194977.080 | 198918.545 | 200401.295 | 201012.920 | 5117.539 |

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
| 64 | available | 1338246.500 | 1352970.150 | 20910.102 | 21140.159 |
| 128 | available | 2479486.083 | 2536461.800 | 19370.985 | 19816.108 |
| 512 | available | 8597763.917 | 8710893.017 | 16792.508 | 17013.463 |
| 2048 | available | 31103316.800 | 31738718.360 | 15187.166 | 15497.421 |
| 17391 | available | 235740896.000 | 241247774.800 | 13555.339 | 13871.990 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1336951.333 | 1359681.967 | 20889.865 | 21245.031 |
| 128 | available | 2494166.667 | 2599651.417 | 19485.677 | 20309.777 |
| 512 | available | 8647788.167 | 8877395.833 | 16890.211 | 17338.664 |
| 2048 | available | 31378141.600 | 36265138.280 | 15321.358 | 17707.587 |
| 17391 | available | 238023500.000 | 251825525.100 | 13686.591 | 14480.221 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs. QRS depth rows include the control-block witness weight for the Merkle path and report the resulting TapBranch hash-operation count; timing estimates still use the measured QRS validation-path timing multiplied by the depth-specific weight-limited input count.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 93.476 | 97.032 | 98.208 | 98.292 |
| QRS invalid fixed-length saturated block | available | 496 | 95.087 | 98.514 | 101.105 | 102.082 |
| Schnorr individual saturated block | available | 17391 | 346.776 | 352.113 | 354.379 | 356.180 |
| Schnorr experimental batch saturated block | available | 17391 | 235.741 | 239.810 | 241.248 | 241.607 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS depth-sensitive saturated block estimates

| Model | QRS Merkle depth | Status | Input WU | Max inputs | TapBranch hashes/block | Median ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | 0 | available | 8059 | 496 | 0 | 93.476 | 98.208 | 98.292 |
| QRS valid saturated block | 1 | available | 8091 | 494 | 494 | 93.099 | 97.812 | 97.895 |
| QRS valid saturated block | 3 | available | 8155 | 490 | 1470 | 92.346 | 97.020 | 97.103 |
| QRS valid saturated block | 128 | available | 12155 | 329 | 42112 | 62.003 | 65.142 | 65.198 |
| QRS invalid fixed-length saturated block | 0 | available | 8059 | 496 | 0 | 95.087 | 101.105 | 102.082 |
| QRS invalid fixed-length saturated block | 1 | available | 8091 | 494 | 494 | 94.704 | 100.698 | 101.671 |
| QRS invalid fixed-length saturated block | 3 | available | 8155 | 490 | 1470 | 93.937 | 99.882 | 100.848 |
| QRS invalid fixed-length saturated block | 128 | available | 12155 | 329 | 42112 | 63.072 | 67.064 | 67.712 |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 98.208
- `experimental_batch_schnorr_p99_ms`: 241.248

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
