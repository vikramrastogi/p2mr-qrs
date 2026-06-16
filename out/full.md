# QRS Native Benchmark Report

Mode: `full`

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
| SLH-DSA-SHA2-128s valid verify | available | 185252.080 | 191671.875 | 227314.294 | 242061.670 | 5345.098 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 196475.840 | 201277.710 | 202352.089 | 202739.590 | 5097.431 |
| BIP-340 Schnorr individual valid verify | available | 20576.746 | 21693.135 | 23981.922 | 24843.133 | 48254.323 |
| BIP-340 Schnorr individual invalid verify | available | 20652.467 | 21001.593 | 21298.929 | 21519.723 | 48340.079 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 14122.361 | 14640.889 | 14915.233 | 14983.818 | 70290.018 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 178838.330 | 184822.915 | 186363.499 | 186876.250 | 5569.887 |
| middle_bitflip | available | 183845.420 | 189230.210 | 190594.127 | 190977.500 | 5442.486 |
| last_bitflip | available | 191722.500 | 194784.375 | 195442.089 | 195687.090 | 5231.015 |
| spread_32_bitflips | available | 196475.840 | 201277.710 | 202352.089 | 202739.590 | 5097.431 |
| all_zero | available | 194544.580 | 198713.330 | 200441.375 | 200941.250 | 5127.223 |
| all_ff | available | 194126.250 | 197566.455 | 198414.287 | 198427.910 | 5171.849 |
| deterministic_garbage | available | 191410.410 | 196398.965 | 196502.546 | 196542.920 | 5230.876 |
| wrong_message | available | 191225.840 | 195621.040 | 196162.250 | 196205.000 | 5235.619 |
| wrong_public_key | available | 184757.500 | 190221.665 | 191866.581 | 192547.080 | 5386.414 |
| min observed invalid fixed-length | available | 178838.330 | 184822.915 | 186363.499 | 186876.250 | 5569.887 |
| best observed invalid fixed-length | available | 178838.330 | 184822.915 | 186363.499 | 186876.250 | 5569.887 |
| median observed invalid fixed-length | available | 191225.840 | 195621.040 | 196162.250 | 196205.000 | 5235.619 |
| p99 observed invalid fixed-length | available | 196475.840 | 201277.710 | 202352.089 | 202739.590 | 5097.431 |
| worst observed invalid fixed-length | available | 196475.840 | 201277.710 | 202352.089 | 202739.590 | 5097.431 |

Worst observed invalid fixed-length case: `spread_32_bitflips`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields using explicit non-consensus prefixes QRS modeled TapLeaf v0, QRS modeled Branch v0, and QRS modeled SigMsg v0; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 369.585 | 386.821 | 389.416 | 390.396 | 2703318.939 |
| qrs_msg construction | available | 751.465 | 786.601 | 789.933 | 790.677 | 1330598.204 |
| verifier valid call | available | 187721.670 | 193867.705 | 197260.579 | 198538.330 | 5311.030 |
| total modeled valid path | available | 189679.160 | 194388.335 | 195821.751 | 196308.750 | 5264.671 |
| invalid structural reject | available | 2.846 | 3.065 | 3.142 | 3.156 | 349533260.345 |
| invalid fixed-length crypto reject | available | 185202.500 | 190019.375 | 193805.288 | 194906.660 | 5395.800 |

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
| 64 | available | 1386826.417 | 1418772.217 | 21669.163 | 22168.316 |
| 128 | available | 2595513.917 | 2684207.617 | 20277.452 | 20970.372 |
| 512 | available | 8973659.750 | 9241354.900 | 17526.679 | 18049.521 |
| 2048 | available | 32269791.600 | 33335495.000 | 15756.734 | 16277.097 |
| 17391 | available | 245601979.000 | 259390808.500 | 14122.361 | 14915.233 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1411347.250 | 1453363.867 | 22052.301 | 22708.810 |
| 128 | available | 2634295.167 | 2695688.917 | 20580.431 | 21060.070 |
| 512 | available | 9026256.917 | 9220792.383 | 17629.408 | 18009.360 |
| 2048 | available | 32596425.000 | 33146491.560 | 15916.223 | 16184.810 |
| 17391 | available | 246557521.000 | 255581716.500 | 14177.306 | 14696.206 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs. QRS depth rows include the control-block witness weight for the Merkle path and report the resulting TapBranch hash-operation count; timing estimates still use the measured QRS validation-path timing multiplied by the depth-specific weight-limited input count.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 91.885 | 95.069 | 112.748 | 120.063 |
| QRS invalid fixed-length saturated block | available | 496 | 97.452 | 99.834 | 100.367 | 100.559 |
| Schnorr individual saturated block | available | 17391 | 357.850 | 377.265 | 417.070 | 432.047 |
| Schnorr experimental batch saturated block | available | 17391 | 245.602 | 254.620 | 259.391 | 260.584 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS depth-sensitive saturated block estimates

| Model | QRS Merkle depth | Status | Input WU | Max inputs | TapBranch hashes/block | Median ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | 0 | available | 8059 | 496 | 0 | 91.885 | 112.748 | 120.063 |
| QRS valid saturated block | 1 | available | 8091 | 494 | 494 | 91.515 | 112.293 | 119.578 |
| QRS valid saturated block | 3 | available | 8155 | 490 | 1470 | 90.774 | 111.384 | 118.610 |
| QRS valid saturated block | 128 | available | 12155 | 329 | 42112 | 60.948 | 74.786 | 79.638 |
| QRS invalid fixed-length saturated block | 0 | available | 8059 | 496 | 0 | 97.452 | 100.367 | 100.559 |
| QRS invalid fixed-length saturated block | 1 | available | 8091 | 494 | 494 | 97.059 | 99.962 | 100.153 |
| QRS invalid fixed-length saturated block | 3 | available | 8155 | 490 | 1470 | 96.273 | 99.153 | 99.342 |
| QRS invalid fixed-length saturated block | 128 | available | 12155 | 329 | 42112 | 64.641 | 66.574 | 66.701 |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 112.748
- `experimental_batch_schnorr_p99_ms`: 259.391

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
