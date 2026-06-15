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
| SLH-DSA-SHA2-128s valid verify | available | 184739.625 | 221104.687 | 229545.937 | 231656.250 | 5178.871 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 183104.125 | 200291.638 | 204104.127 | 205057.250 | 5353.792 |
| BIP-340 Schnorr individual valid verify | available | 20607.584 | 21254.500 | 21569.567 | 21648.334 | 48490.009 |
| BIP-340 Schnorr individual invalid verify | available | 20631.062 | 20697.636 | 20714.627 | 20718.875 | 48721.661 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13763.776 | 13887.940 | 13908.278 | 13913.363 | 72866.759 |

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 356.062 | 381.096 | 383.836 | 384.521 | 2744067.131 |
| qrs_msg construction | available | 719.562 | 763.846 | 772.886 | 775.146 | 1368457.065 |
| verifier valid call | available | 192046.875 | 202011.425 | 204443.885 | 205052.000 | 5158.180 |
| total modeled valid path | available | 193145.875 | 202487.987 | 204114.197 | 204520.750 | 5141.663 |
| invalid structural reject | available | 2.729 | 2.750 | 2.750 | 2.750 | 366003503.176 |
| invalid fixed-length crypto reject | available | 192578.125 | 208523.450 | 212879.690 | 213968.750 | 5122.498 |

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
| 64 | available | 1365055.333 | 1435725.667 | 21328.990 | 22433.214 |
| 128 | available | 2551819.667 | 2577590.653 | 19936.091 | 20137.427 |
| 512 | available | 8862597.333 | 8944135.893 | 17309.760 | 17469.015 |
| 2048 | available | 31436458.000 | 31582684.000 | 15349.833 | 15421.232 |
| 17391 | available | 239365833.000 | 241878867.000 | 13763.776 | 13908.278 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1365402.667 | 1425305.547 | 21334.417 | 22270.399 |
| 128 | available | 2597027.667 | 2680100.000 | 20289.279 | 20938.281 |
| 512 | available | 8943500.000 | 8994929.893 | 17467.773 | 17568.222 |
| 2048 | available | 31117125.000 | 31487686.640 | 15193.909 | 15374.847 |
| 17391 | available | 237841125.000 | 243873259.360 | 13676.104 | 14022.958 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 91.631 | 109.668 | 113.855 | 114.901 |
| QRS invalid fixed-length saturated block | available | 496 | 90.820 | 99.345 | 101.236 | 101.708 |
| Schnorr individual saturated block | available | 17391 | 358.386 | 369.637 | 375.116 | 376.486 |
| Schnorr experimental batch saturated block | available | 17391 | 239.366 | 241.525 | 241.879 | 241.967 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 113.855
- `experimental_batch_schnorr_p99_ms`: 241.879

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
