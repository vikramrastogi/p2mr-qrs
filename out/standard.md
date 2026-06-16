# QRS Native Benchmark Report

Mode: `standard`

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
| SLH-DSA-SHA2-128s valid verify | available | 177075.500 | 179941.800 | 181342.535 | 181692.719 | 5670.165 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 195760.406 | 200361.725 | 201474.445 | 201752.625 | 5102.398 |
| BIP-340 Schnorr individual valid verify | available | 19832.797 | 20334.640 | 20350.925 | 20355.693 | 50165.217 |
| BIP-340 Schnorr individual invalid verify | available | 20239.578 | 20599.566 | 20624.835 | 20631.229 | 49444.258 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13999.061 | 14109.463 | 14132.321 | 14138.036 | 71430.133 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 180183.594 | 185476.953 | 185776.641 | 185851.562 | 5530.379 |
| middle_bitflip | available | 188760.438 | 194823.962 | 200763.742 | 202248.688 | 5291.890 |
| last_bitflip | available | 179307.281 | 184445.200 | 185511.965 | 185778.656 | 5580.508 |
| spread_32_bitflips | available | 195605.469 | 198873.434 | 199417.387 | 199553.375 | 5114.224 |
| all_zero | available | 189089.844 | 194895.587 | 195432.242 | 195566.406 | 5281.051 |
| all_ff | available | 195760.406 | 200361.725 | 201474.445 | 201752.625 | 5102.398 |
| deterministic_garbage | available | 185898.438 | 191586.972 | 194092.394 | 194718.750 | 5364.927 |
| wrong_message | available | 182273.438 | 186291.666 | 186444.808 | 186483.094 | 5503.882 |
| wrong_public_key | available | 189490.875 | 193380.081 | 195555.191 | 196098.969 | 5274.906 |
| min observed invalid fixed-length | available | 179307.281 | 184445.200 | 185511.965 | 185778.656 | 5580.508 |
| best observed invalid fixed-length | available | 179307.281 | 184445.200 | 185511.965 | 185778.656 | 5580.508 |
| median observed invalid fixed-length | available | 189089.844 | 194895.587 | 195432.242 | 195566.406 | 5281.051 |
| p99 observed invalid fixed-length | available | 195760.406 | 200361.725 | 201474.445 | 201752.625 | 5102.398 |
| worst observed invalid fixed-length | available | 195760.406 | 200361.725 | 201474.445 | 201752.625 | 5102.398 |

Worst observed invalid fixed-length case: `all_ff`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields using explicit non-consensus prefixes QRS modeled TapLeaf v0, QRS modeled Branch v0, and QRS modeled SigMsg v0; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 365.198 | 379.361 | 379.472 | 379.500 | 2725899.370 |
| qrs_msg construction | available | 739.906 | 767.020 | 769.946 | 770.677 | 1347534.248 |
| verifier valid call | available | 190092.438 | 195761.606 | 196062.746 | 196138.031 | 5266.560 |
| total modeled valid path | available | 192420.562 | 195791.663 | 195806.258 | 195809.906 | 5237.392 |
| invalid structural reject | available | 2.849 | 3.323 | 3.498 | 3.542 | 339102790.533 |
| invalid fixed-length crypto reject | available | 192006.531 | 198032.559 | 200234.637 | 200785.156 | 5200.037 |

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
| 64 | available | 1379097.167 | 1446838.883 | 21548.393 | 22606.858 |
| 128 | available | 2595666.667 | 2649184.083 | 20278.646 | 20696.751 |
| 512 | available | 8967270.833 | 9081961.850 | 17514.201 | 17738.207 |
| 2048 | available | 32209847.333 | 32433050.100 | 15727.465 | 15836.450 |
| 17391 | available | 243457667.000 | 245775199.700 | 13999.061 | 14132.321 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1383264.000 | 1441356.883 | 21613.500 | 22521.201 |
| 128 | available | 2599986.167 | 2667209.017 | 20312.392 | 20837.570 |
| 512 | available | 9009979.167 | 9124679.117 | 17597.616 | 17821.639 |
| 2048 | available | 32269555.667 | 36319022.233 | 15756.619 | 17733.898 |
| 17391 | available | 244720708.000 | 247291366.600 | 14071.687 | 14219.502 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs. QRS depth rows include the control-block witness weight for the Merkle path and report the resulting TapBranch hash-operation count; timing estimates still use the measured QRS validation-path timing multiplied by the depth-specific weight-limited input count.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 87.829 | 89.251 | 89.946 | 90.120 |
| QRS invalid fixed-length saturated block | available | 496 | 97.097 | 99.379 | 99.931 | 100.069 |
| Schnorr individual saturated block | available | 17391 | 344.912 | 353.640 | 353.923 | 354.006 |
| Schnorr experimental batch saturated block | available | 17391 | 243.458 | 245.378 | 245.775 | 245.875 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS depth-sensitive saturated block estimates

| Model | QRS Merkle depth | Status | Input WU | Max inputs | TapBranch hashes/block | Median ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | 0 | available | 8059 | 496 | 0 | 87.829 | 89.946 | 90.120 |
| QRS valid saturated block | 1 | available | 8091 | 494 | 494 | 87.475 | 89.583 | 89.756 |
| QRS valid saturated block | 3 | available | 8155 | 490 | 1470 | 86.767 | 88.858 | 89.029 |
| QRS valid saturated block | 128 | available | 12155 | 329 | 42112 | 58.258 | 59.662 | 59.777 |
| QRS invalid fixed-length saturated block | 0 | available | 8059 | 496 | 0 | 97.097 | 99.931 | 100.069 |
| QRS invalid fixed-length saturated block | 1 | available | 8091 | 494 | 494 | 96.706 | 99.528 | 99.666 |
| QRS invalid fixed-length saturated block | 3 | available | 8155 | 490 | 1470 | 95.923 | 98.722 | 98.859 |
| QRS invalid fixed-length saturated block | 128 | available | 12155 | 329 | 42112 | 64.405 | 66.285 | 66.377 |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 89.946
- `experimental_batch_schnorr_p99_ms`: 245.775

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
