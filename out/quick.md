# QRS Native Benchmark Report

Mode: `quick`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_commit`: record with git rev-parse HEAD in published runs
- `build_type`: Release/NDEBUG
- `compiler`: clang 17.0.0 (clang-1700.6.4.2)
- `compiler_flags`: CMake target: -O3 -Wall -Wextra -Wpedantic
- `cpu_features`: hw.optional.arm.FEAT_AES=1, hw.optional.arm.FEAT_PMULL=1, hw.optional.arm.FEAT_SHA1=1, hw.optional.arm.FEAT_SHA256=1, hw.optional.arm.FEAT_SHA3=1, hw.optional.arm.FEAT_SHA512=1, hw.optional.arm.FEAT_LSE=1, hw.optional.arm.FEAT_FP16=1
- `cpu_frequency`: unavailable
- `cpu_model`: Apple M2 Pro
- `frequency_scaling`: not controlled by harness; reviewer must record governor/power mode
- `os`: Darwin 25.3.0 arm64
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
| SLH-DSA-SHA2-128s valid verify | available | 186692.625 | 191069.763 | 191559.753 | 191682.250 | 5390.079 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 418937.500 | 490164.062 | 491907.812 | 492343.750 | 2773.381 |
| BIP-340 Schnorr individual valid verify | available | 20593.833 | 33517.146 | 42241.662 | 44422.791 | 43549.588 |
| BIP-340 Schnorr individual invalid verify | available | 20149.562 | 20353.896 | 20370.229 | 20374.312 | 49681.362 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13710.523 | 13777.901 | 13787.733 | 13790.191 | 72844.946 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 198921.875 | 201927.550 | 202318.810 | 202416.625 | 5036.633 |
| middle_bitflip | available | 177088.625 | 182415.625 | 182558.125 | 182593.750 | 5599.044 |
| last_bitflip | available | 183703.125 | 200243.762 | 203598.753 | 204437.500 | 5358.232 |
| spread_32_bitflips | available | 185229.125 | 193890.075 | 196311.315 | 196916.625 | 5398.133 |
| all_zero | available | 192599.000 | 196315.625 | 196338.125 | 196343.750 | 5181.967 |
| all_ff | available | 418937.500 | 490164.062 | 491907.812 | 492343.750 | 2773.381 |
| deterministic_garbage | available | 197406.250 | 395680.800 | 413007.060 | 417338.625 | 3890.228 |
| best observed invalid fixed-length | available | 177088.625 | 182415.625 | 182558.125 | 182593.750 | 5599.044 |
| median observed invalid fixed-length | available | 198921.875 | 201927.550 | 202318.810 | 202416.625 | 5036.633 |
| worst observed invalid fixed-length | available | 418937.500 | 490164.062 | 491907.812 | 492343.750 | 2773.381 |

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 361.521 | 369.367 | 372.007 | 372.667 | 2815107.556 |
| qrs_msg construction | available | 726.312 | 734.710 | 734.875 | 734.917 | 1378529.405 |
| verifier valid call | available | 177692.750 | 190403.137 | 192130.628 | 192562.500 | 5564.779 |
| total modeled valid path | available | 180770.750 | 183562.950 | 184229.190 | 184395.750 | 5514.142 |
| invalid structural reject | available | 2.771 | 2.771 | 2.771 | 2.771 | 361682339.568 |
| invalid fixed-length crypto reject | available | 193640.625 | 197819.775 | 198859.755 | 199119.750 | 5158.952 |

## Batch Schnorr Baseline

- `reviewed_public_api_status`: unavailable
- `reviewed_public_api_reason`: Bundled upstream bitcoin-core/secp256k1 commit aea86bc3509f3ceb5c047fe6567bf8910f72a728 exposes only the public secp256k1_schnorrsig_verify API for BIP-340; its schnorrsig tests still mark verify_batch as TODO, and git ls-remote found no upstream heads matching batch/schnorr/bip340 for a reviewed experimental baseline. No fake batch baseline is synthesized.
- `experimental_status`: available
- `experimental_label`: Experimental native BIP-340 batch baseline; not a reviewed public libsecp256k1 API.
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
| 64 | available | 1357611.333 | 1424275.653 | 21212.677 | 22254.307 |
| 128 | available | 2539916.667 | 2565703.773 | 19843.099 | 20044.561 |
| 512 | available | 8730416.667 | 8774042.227 | 17051.595 | 17136.801 |
| 2048 | available | 31164250.000 | 31592328.000 | 15216.919 | 15425.941 |
| 17391 | available | 238439708.000 | 239782463.960 | 13710.523 | 13787.733 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1376847.000 | 1392071.213 | 21513.234 | 21751.113 |
| 128 | available | 2549694.333 | 2585005.107 | 19919.487 | 20195.352 |
| 512 | available | 8948041.667 | 15723297.213 | 17476.644 | 30709.565 |
| 2048 | available | 31533750.000 | 31881606.960 | 15397.339 | 15567.191 |
| 17391 | available | 239504458.000 | 305194226.320 | 13771.747 | 17548.975 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 92.600 | 94.771 | 95.014 | 95.074 |
| QRS invalid fixed-length saturated block | available | 496 | 207.793 | 243.121 | 243.986 | 244.202 |
| Schnorr individual saturated block | available | 17391 | 358.147 | 582.897 | 734.625 | 772.557 |
| Schnorr experimental batch saturated block | available | 17391 | 238.440 | 239.611 | 239.782 | 239.825 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 95.014
- `experimental_batch_schnorr_p99_ms`: 239.782

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
