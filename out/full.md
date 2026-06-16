# QRS Native Benchmark Report

Mode: `full`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: 3821510b6bcc223e0d716dc65021f249afca3e68
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
- `git_commit`: 3821510b6bcc223e0d716dc65021f249afca3e68
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

## Raw Timing Table

Timing statistics are computed as the median of per-batch means, p95 of per-batch means, p99 of per-batch means, and max of per-batch means; they are not individual operation latency samples.

| Benchmark | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| SLH-DSA-SHA2-128s valid verify | available | 180995.830 | 182876.045 | 183240.583 | 183358.330 | 5523.675 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 191208.330 | 192980.205 | 193366.829 | 193399.580 | 5229.623 |
| BIP-340 Schnorr individual valid verify | available | 19745.348 | 19844.200 | 19872.278 | 19886.990 | 50625.247 |
| BIP-340 Schnorr individual invalid verify | available | 19815.852 | 20048.928 | 21820.955 | 22718.188 | 50237.830 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13659.499 | 13798.460 | 13805.013 | 13806.652 | 73230.612 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 178015.000 | 179561.460 | 180000.288 | 180126.660 | 5616.886 |
| middle_bitflip | available | 183753.330 | 185329.585 | 186705.625 | 187266.250 | 5438.668 |
| last_bitflip | available | 181370.420 | 182927.290 | 183890.207 | 184223.330 | 5513.041 |
| spread_32_bitflips | available | 178858.330 | 180093.750 | 180348.831 | 180457.080 | 5588.373 |
| all_zero | available | 180489.170 | 181924.165 | 184125.125 | 185063.750 | 5538.085 |
| all_ff | available | 181104.160 | 182442.920 | 182467.088 | 182469.590 | 5522.423 |
| deterministic_garbage | available | 188000.420 | 189322.915 | 190174.713 | 190448.340 | 5324.665 |
| wrong_message | available | 191208.330 | 192980.205 | 193366.829 | 193399.580 | 5229.623 |
| wrong_public_key | available | 178003.330 | 180363.130 | 180953.457 | 181162.080 | 5609.732 |
| min observed invalid fixed-length | available | 178015.000 | 179561.460 | 180000.288 | 180126.660 | 5616.886 |
| best observed invalid fixed-length | available | 178015.000 | 179561.460 | 180000.288 | 180126.660 | 5616.886 |
| median observed invalid fixed-length | available | 181370.420 | 182927.290 | 183890.207 | 184223.330 | 5513.041 |
| p99 observed invalid fixed-length | available | 191208.330 | 192980.205 | 193366.829 | 193399.580 | 5229.623 |
| worst observed invalid fixed-length | available | 191208.330 | 192980.205 | 193366.829 | 193399.580 | 5229.623 |

Worst observed invalid fixed-length case: `wrong_message`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 349.748 | 358.791 | 359.648 | 359.944 | 2862496.306 |
| qrs_msg construction | available | 713.308 | 721.061 | 723.607 | 723.996 | 1403849.872 |
| verifier valid call | available | 179710.830 | 180865.415 | 181126.711 | 181233.340 | 5566.109 |
| total modeled valid path | available | 180600.420 | 183696.245 | 185730.535 | 185919.160 | 5532.041 |
| invalid structural reject | available | 2.644 | 2.675 | 2.679 | 2.679 | 377836592.986 |
| invalid fixed-length crypto reject | available | 198852.080 | 200378.750 | 201011.169 | 201206.670 | 5023.629 |

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
| 64 | available | 1330909.667 | 1338245.850 | 20795.464 | 20910.091 |
| 128 | available | 2493798.583 | 2584804.883 | 19482.801 | 20193.788 |
| 512 | available | 8659548.583 | 8695057.617 | 16913.181 | 16982.534 |
| 2048 | available | 31154408.400 | 31343283.320 | 15212.113 | 15304.338 |
| 17391 | available | 237552354.000 | 240082987.800 | 13659.499 | 13805.013 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1344642.333 | 1365781.983 | 21010.036 | 21340.343 |
| 128 | available | 2509208.333 | 2587929.917 | 19603.190 | 20218.202 |
| 512 | available | 8725420.167 | 10505195.883 | 17041.836 | 20517.961 |
| 2048 | available | 31454108.200 | 43012425.000 | 15358.451 | 21002.161 |
| 17391 | available | 234689625.000 | 255292941.600 | 13494.890 | 14679.601 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 89.774 | 90.707 | 90.887 | 90.946 |
| QRS invalid fixed-length saturated block | available | 496 | 94.839 | 95.718 | 95.910 | 95.926 |
| Schnorr individual saturated block | available | 17391 | 343.391 | 345.110 | 345.599 | 345.855 |
| Schnorr experimental batch saturated block | available | 17391 | 237.552 | 239.969 | 240.083 | 240.111 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 90.887
- `experimental_batch_schnorr_p99_ms`: 240.083

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
