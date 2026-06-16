# QRS Native Benchmark Report

Mode: `quick`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: 6c567227a9f7e784f3d18d14705c6e3d947a7aba
- `build_type`: Release/NDEBUG
- `compiler`: clang 17.0.0 (clang-1700.6.4.2)
- `compiler_flags`: CMake target: -O3 -Wall -Wextra -Wpedantic
- `cpu_features`: hw.optional.arm.FEAT_AES=1, hw.optional.arm.FEAT_PMULL=1, hw.optional.arm.FEAT_SHA1=1, hw.optional.arm.FEAT_SHA256=1, hw.optional.arm.FEAT_SHA3=1, hw.optional.arm.FEAT_SHA512=1, hw.optional.arm.FEAT_LSE=1, hw.optional.arm.FEAT_FP16=1
- `cpu_frequency`: unavailable
- `cpu_model`: Apple M2 Pro
- `frequency_scaling`: not controlled by harness; reviewer must record governor/power mode
- `git_commit`: 6c567227a9f7e784f3d18d14705c6e3d947a7aba
- `os`: Darwin 25.3.0 arm64
- `working_tree_dirty`: false
- `OpenSSL`: OpenSSL 3.6.1 27 Jan 2026
- `OpenSSL provider`: default
- `SLH-DSA mode`: EVP direct message signing/verification, no prehash digest, context-string explicitly set to empty
- `SLH-DSA public key bytes`: 32
- `SLH-DSA signature bytes`: 7856
- `libsecp256k1`: aea86bc3509f3ceb5c047fe6567bf8910f72a728

## Raw Timing Table

Timing statistics are computed as the median of per-batch means, p95 of per-batch means, p99 of per-batch means, and max of per-batch means; they are not individual operation latency samples.

| Benchmark | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| SLH-DSA-SHA2-128s valid verify | available | 191885.375 | 200884.912 | 202743.682 | 203208.375 | 5211.767 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 201385.375 | 205745.325 | 205886.565 | 205921.875 | 4986.587 |
| BIP-340 Schnorr individual valid verify | available | 20275.209 | 20528.156 | 20604.298 | 20623.334 | 49375.203 |
| BIP-340 Schnorr individual invalid verify | available | 20056.834 | 20155.146 | 20157.196 | 20157.709 | 49835.058 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13686.794 | 13757.897 | 13768.768 | 13771.486 | 73040.264 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 192562.500 | 195269.738 | 195274.747 | 195276.000 | 5256.982 |
| middle_bitflip | available | 180687.500 | 183905.700 | 184601.940 | 184776.000 | 5556.222 |
| last_bitflip | available | 188328.125 | 197050.562 | 198126.812 | 198395.875 | 5269.452 |
| spread_32_bitflips | available | 179833.250 | 186801.575 | 186822.815 | 186828.125 | 5491.295 |
| all_zero | available | 193375.000 | 204422.988 | 205642.997 | 205948.000 | 5109.236 |
| all_ff | available | 187041.625 | 189810.938 | 190212.188 | 190312.500 | 5351.128 |
| deterministic_garbage | available | 170937.500 | 177560.938 | 178112.188 | 178250.000 | 5816.244 |
| wrong_message | available | 184572.875 | 197403.125 | 200830.625 | 201687.500 | 5375.699 |
| wrong_public_key | available | 201385.375 | 205745.325 | 205886.565 | 205921.875 | 4986.587 |
| min observed invalid fixed-length | available | 170937.500 | 177560.938 | 178112.188 | 178250.000 | 5816.244 |
| best observed invalid fixed-length | available | 170937.500 | 177560.938 | 178112.188 | 178250.000 | 5816.244 |
| median observed invalid fixed-length | available | 192562.500 | 195269.738 | 195274.747 | 195276.000 | 5256.982 |
| p99 observed invalid fixed-length | available | 201385.375 | 205745.325 | 205886.565 | 205921.875 | 4986.587 |
| worst observed invalid fixed-length | available | 201385.375 | 205745.325 | 205886.565 | 205921.875 | 4986.587 |

Worst observed invalid fixed-length case: `wrong_public_key`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 361.833 | 369.460 | 370.475 | 370.729 | 2753918.679 |
| qrs_msg construction | available | 745.792 | 780.461 | 788.826 | 790.917 | 1333290.922 |
| verifier valid call | available | 195885.500 | 434402.125 | 499809.625 | 516161.500 | 3940.320 |
| total modeled valid path | available | 420588.625 | 682865.625 | 704773.125 | 710250.000 | 2163.836 |
| invalid structural reject | available | 6.375 | 6.571 | 6.631 | 6.646 | 155919367.413 |
| invalid fixed-length crypto reject | available | 251302.125 | 516497.987 | 548895.497 | 556994.875 | 3174.790 |

## Batch Schnorr Baseline

- `reviewed_public_api_status`: unavailable
- `reviewed_public_api_reason`: Bundled upstream bitcoin-core/secp256k1 commit aea86bc3509f3ceb5c047fe6567bf8910f72a728 exposes only the public secp256k1_schnorrsig_verify API for BIP-340; its schnorrsig tests still mark verify_batch as TODO, and git ls-remote found no upstream heads matching batch/schnorr/bip340 for a reviewed experimental baseline. No fake batch baseline is synthesized.
- `experimental_status`: available
- `experimental_label`: Experimental native BIP-340 batch baseline; not a reviewed public libsecp256k1 API.
- BIP-340 challenge self-test: `available` - BIP-340 challenge self-test passed: libsecp256k1 tagged SHA helper matched an independent tagged_hash construction over deterministic inputs.
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
| 64 | available | 1337430.667 | 1404074.440 | 20897.354 | 21938.663 |
| 128 | available | 2510764.000 | 2519677.440 | 19615.344 | 19684.980 |
| 512 | available | 8763972.333 | 8795110.453 | 17117.133 | 17177.950 |
| 2048 | available | 31206792.000 | 31376548.320 | 15237.691 | 15320.580 |
| 17391 | available | 238027041.000 | 239452652.000 | 13686.794 | 13768.768 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1321028.000 | 1398409.893 | 20641.062 | 21850.155 |
| 128 | available | 2521513.667 | 2569524.107 | 19699.326 | 20074.407 |
| 512 | available | 8721000.000 | 8783686.000 | 17033.203 | 17155.637 |
| 2048 | available | 31353167.000 | 31448547.000 | 15309.164 | 15355.736 |
| 17391 | available | 240281166.000 | 269011955.000 | 13816.409 | 15468.458 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 95.175 | 99.639 | 100.561 | 100.791 |
| QRS invalid fixed-length saturated block | available | 496 | 99.887 | 102.050 | 102.120 | 102.137 |
| Schnorr individual saturated block | available | 17391 | 352.606 | 357.005 | 358.329 | 358.660 |
| Schnorr experimental batch saturated block | available | 17391 | 238.027 | 239.264 | 239.453 | 239.500 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 100.561
- `experimental_batch_schnorr_p99_ms`: 239.453

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
