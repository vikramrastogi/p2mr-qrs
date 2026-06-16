# QRS Native Benchmark Report

Mode: `full`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: 598148bf7bd0d4b491488868cead23f9ec7b7682
- `build_type`: Release/NDEBUG
- `compiler`: clang 17.0.0 (clang-1700.6.4.2)
- `compiler_flags`: CMake target: -O3 -Wall -Wextra -Wpedantic
- `cpu_features`: hw.optional.arm.FEAT_AES=1, hw.optional.arm.FEAT_PMULL=1, hw.optional.arm.FEAT_SHA1=1, hw.optional.arm.FEAT_SHA256=1, hw.optional.arm.FEAT_SHA3=1, hw.optional.arm.FEAT_SHA512=1, hw.optional.arm.FEAT_LSE=1, hw.optional.arm.FEAT_FP16=1
- `cpu_frequency`: unavailable
- `cpu_model`: Apple M2 Pro
- `frequency_scaling`: not controlled by harness; reviewer must record governor/power mode
- `git_commit`: 598148bf7bd0d4b491488868cead23f9ec7b7682
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
| SLH-DSA-SHA2-128s valid verify | available | 182282.080 | 184019.380 | 185557.546 | 186152.920 | 5489.743 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 200899.160 | 411592.710 | 516527.294 | 558230.420 | 4060.835 |
| BIP-340 Schnorr individual valid verify | available | 20031.171 | 20686.089 | 22950.123 | 23390.827 | 49662.151 |
| BIP-340 Schnorr individual invalid verify | available | 20154.088 | 22047.233 | 27181.172 | 29475.487 | 48761.538 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13848.142 | 13953.307 | 13955.617 | 13956.195 | 72313.266 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 189567.910 | 194536.875 | 195416.292 | 195765.420 | 5263.835 |
| middle_bitflip | available | 195842.080 | 201150.835 | 205320.543 | 206362.920 | 5103.691 |
| last_bitflip | available | 210452.500 | 260701.460 | 358312.214 | 393328.340 | 4692.869 |
| spread_32_bitflips | available | 200899.160 | 411592.710 | 516527.294 | 558230.420 | 4060.835 |
| all_zero | available | 189692.920 | 191909.165 | 193621.998 | 194148.750 | 5271.220 |
| all_ff | available | 183331.250 | 187246.250 | 188116.332 | 188448.330 | 5444.197 |
| deterministic_garbage | available | 178155.000 | 180974.795 | 182431.213 | 182778.340 | 5618.316 |
| wrong_message | available | 178862.920 | 181746.250 | 182980.126 | 183475.000 | 5590.598 |
| wrong_public_key | available | 185887.910 | 189587.500 | 190080.000 | 190155.000 | 5388.836 |
| min observed invalid fixed-length | available | 178155.000 | 180974.795 | 182431.213 | 182778.340 | 5618.316 |
| best observed invalid fixed-length | available | 178155.000 | 180974.795 | 182431.213 | 182778.340 | 5618.316 |
| median observed invalid fixed-length | available | 189692.920 | 191909.165 | 193621.998 | 194148.750 | 5271.220 |
| p99 observed invalid fixed-length | available | 200899.160 | 411592.710 | 516527.294 | 558230.420 | 4060.835 |
| worst observed invalid fixed-length | available | 200899.160 | 411592.710 | 516527.294 | 558230.420 | 4060.835 |

Worst observed invalid fixed-length case: `spread_32_bitflips`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 368.667 | 376.484 | 380.295 | 380.708 | 2714302.168 |
| qrs_msg construction | available | 736.746 | 748.231 | 750.293 | 750.844 | 1354906.581 |
| verifier valid call | available | 190855.000 | 192593.330 | 192749.419 | 192790.420 | 5234.858 |
| total modeled valid path | available | 190654.590 | 193125.835 | 193806.957 | 194065.830 | 5246.059 |
| invalid structural reject | available | 2.754 | 3.006 | 3.569 | 3.779 | 356458481.498 |
| invalid fixed-length crypto reject | available | 185212.500 | 187462.710 | 187555.626 | 187556.250 | 5406.517 |

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
| 64 | available | 1368357.583 | 1406565.250 | 21380.587 | 21977.582 |
| 128 | available | 2538812.500 | 2859344.417 | 19834.473 | 22338.628 |
| 512 | available | 8743250.000 | 8875678.450 | 17076.660 | 17335.309 |
| 2048 | available | 31583691.600 | 31932713.440 | 15421.724 | 15592.145 |
| 17391 | available | 240833041.500 | 242702141.600 | 13848.142 | 13955.617 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1351701.417 | 1384629.867 | 21120.335 | 21634.842 |
| 128 | available | 2538829.833 | 4006145.817 | 19834.608 | 31298.014 |
| 512 | available | 8762100.750 | 8959132.667 | 17113.478 | 17498.306 |
| 2048 | available | 31735608.400 | 32099748.200 | 15495.903 | 15673.705 |
| 17391 | available | 242233416.500 | 244930954.000 | 13928.665 | 14083.776 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 90.412 | 91.274 | 92.037 | 92.332 |
| QRS invalid fixed-length saturated block | available | 496 | 99.646 | 204.150 | 256.198 | 276.882 |
| Schnorr individual saturated block | available | 17391 | 348.362 | 359.752 | 399.126 | 406.790 |
| Schnorr experimental batch saturated block | available | 17391 | 240.833 | 242.662 | 242.702 | 242.712 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 92.037
- `experimental_batch_schnorr_p99_ms`: 242.702

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
