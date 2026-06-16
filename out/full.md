# QRS Native Benchmark Report

Mode: `full`

## Conclusion

The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.

## Environment

- `benchmark_binary_build_mode`: Release/NDEBUG
- `benchmark_commit`: f09a5053a61b704bd8eb9b9742122b0ddb625cd5
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
- `git_commit`: f09a5053a61b704bd8eb9b9742122b0ddb625cd5
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

## Raw Timing Table

Timing statistics are computed as the median of per-batch means, p95 of per-batch means, p99 of per-batch means, and max of per-batch means; they are not individual operation latency samples.

| Benchmark | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| SLH-DSA-SHA2-128s valid verify | available | 197606.250 | 199552.705 | 200388.580 | 200589.580 | 5072.416 |
| SLH-DSA-SHA2-128s invalid fixed-length verify | available | 197078.750 | 213687.085 | 236391.044 | 245450.420 | 5022.079 |
| BIP-340 Schnorr individual valid verify | available | 20301.204 | 22295.833 | 25579.846 | 27725.879 | 48414.505 |
| BIP-340 Schnorr individual invalid verify | available | 20308.669 | 24312.586 | 25387.168 | 25616.173 | 48104.381 |
| BIP-340 Schnorr reviewed-public batch per-signature verify | unavailable | | | | | |
| BIP-340 Schnorr experimental batch primary per-signature verify | available | 13961.688 | 17727.378 | 18015.226 | 18087.188 | 69438.584 |

## SLH-DSA Invalid Fixed-Length Suite

Each case keeps the signature length at 7,856 bytes and is checked to fail before timing. The compatibility `invalid_fixed_length_verify` value in JSON is the worst observed p99 case from this suite.

| Case | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| byte0_bitflip | available | 188240.420 | 191782.085 | 193419.545 | 194105.420 | 5302.321 |
| middle_bitflip | available | 186442.080 | 189761.875 | 190380.250 | 190491.250 | 5360.935 |
| last_bitflip | available | 199056.250 | 201620.000 | 202599.250 | 202986.250 | 5029.636 |
| spread_32_bitflips | available | 183933.750 | 187931.040 | 189720.542 | 190344.170 | 5445.643 |
| all_zero | available | 177257.500 | 183459.165 | 183967.167 | 184102.920 | 5626.984 |
| all_ff | available | 202330.420 | 207154.380 | 207759.832 | 207857.080 | 4934.031 |
| deterministic_garbage | available | 197078.750 | 213687.085 | 236391.044 | 245450.420 | 5022.079 |
| wrong_message | available | 177488.330 | 187847.085 | 193129.838 | 193723.340 | 5603.039 |
| wrong_public_key | available | 182827.910 | 186049.375 | 186598.206 | 186715.830 | 5464.163 |
| min observed invalid fixed-length | available | 177257.500 | 183459.165 | 183967.167 | 184102.920 | 5626.984 |
| best observed invalid fixed-length | available | 177257.500 | 183459.165 | 183967.167 | 184102.920 | 5626.984 |
| median observed invalid fixed-length | available | 177488.330 | 187847.085 | 193129.838 | 193723.340 | 5603.039 |
| p99 observed invalid fixed-length | available | 197078.750 | 213687.085 | 236391.044 | 245450.420 | 5022.079 |
| worst observed invalid fixed-length | available | 197078.750 | 213687.085 | 236391.044 | 245450.420 | 5022.079 |

Worst observed invalid fixed-length case: `deterministic_garbage`.

## QRS Validation Path Model

- `status`: available
- `scope`: Modeled validation path only; not Bitcoin Core consensus integration.
- `qrs_msg_scope`: Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.

| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | p99 batch-mean ns | Max batch-mean ns | Ops/sec |
|---|---:|---:|---:|---:|---:|---:|
| structural checks | available | 367.023 | 378.558 | 380.313 | 380.810 | 2723294.597 |
| qrs_msg construction | available | 734.260 | 749.472 | 750.553 | 750.640 | 1360975.650 |
| verifier valid call | available | 192692.920 | 217171.670 | 237787.294 | 246284.170 | 5067.562 |
| total modeled valid path | available | 191689.580 | 391963.960 | 411042.295 | 411947.920 | 4386.807 |
| invalid structural reject | available | 2.850 | 5.928 | 7.716 | 8.073 | 303965077.354 |
| invalid fixed-length crypto reject | available | 194753.750 | 197741.460 | 202522.626 | 204551.250 | 5127.332 |

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
| 64 | available | 1392229.167 | 1487389.617 | 21753.581 | 23240.463 |
| 128 | available | 2574538.167 | 2735274.317 | 20113.579 | 21369.331 |
| 512 | available | 8979381.917 | 13127198.617 | 17537.855 | 25639.060 |
| 2048 | available | 32025250.000 | 49180941.600 | 15637.329 | 24014.132 |
| 17391 | available | 242807708.000 | 313302799.900 | 13961.688 | 18015.226 |

## Experimental Invalid Batch Timing

| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |
|---:|---:|---:|---:|---:|---:|
| 64 | available | 1368145.833 | 1495584.717 | 21377.279 | 23368.511 |
| 128 | available | 2547298.667 | 5700259.017 | 19900.771 | 44533.274 |
| 512 | available | 8842086.833 | 8931633.333 | 17269.701 | 17444.596 |
| 2048 | available | 31574375.000 | 32188433.280 | 15417.175 | 15717.008 |
| 17391 | available | 242355208.000 | 283985008.500 | 13935.668 | 16329.424 |

## Block Model

Simplified per-input saturation approximation. Real blocks include transaction overhead and outputs. QRS depth rows include the control-block witness weight for the Merkle path and report the resulting TapBranch hash-operation count; timing estimates still use the measured QRS validation-path timing multiplied by the depth-specific weight-limited input count.

| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | available | 496 | 98.013 | 98.978 | 99.393 | 99.492 |
| QRS invalid fixed-length saturated block | available | 496 | 97.751 | 105.989 | 117.250 | 121.743 |
| Schnorr individual saturated block | available | 17391 | 353.058 | 387.747 | 444.859 | 482.181 |
| Schnorr experimental batch saturated block | available | 17391 | 242.808 | 308.297 | 313.303 | 314.554 |
| Schnorr reviewed-public batch saturated block | unavailable | 17391 | | | | |

## QRS depth-sensitive saturated block estimates

| Model | QRS Merkle depth | Status | Input WU | Max inputs | TapBranch hashes/block | Median ms | p99 ms | Max ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| QRS valid saturated block | 0 | available | 8059 | 496 | 0 | 98.013 | 99.393 | 99.492 |
| QRS valid saturated block | 1 | available | 8091 | 494 | 494 | 97.617 | 98.992 | 99.091 |
| QRS valid saturated block | 3 | available | 8155 | 490 | 1470 | 96.827 | 98.190 | 98.289 |
| QRS valid saturated block | 128 | available | 12155 | 329 | 42112 | 65.012 | 65.928 | 65.994 |
| QRS invalid fixed-length saturated block | 0 | available | 8059 | 496 | 0 | 97.751 | 117.250 | 121.743 |
| QRS invalid fixed-length saturated block | 1 | available | 8091 | 494 | 494 | 97.357 | 116.777 | 121.253 |
| QRS invalid fixed-length saturated block | 3 | available | 8155 | 490 | 1470 | 96.569 | 115.832 | 120.271 |
| QRS invalid fixed-length saturated block | 128 | available | 12155 | 329 | 42112 | 64.839 | 77.773 | 80.753 |

## QRS vs experimental batch Schnorr

- `qrs_valid_p99_ms`: 99.393
- `experimental_batch_schnorr_p99_ms`: 313.303

## Caveats

- One machine is not consensus readiness evidence by itself.
- Frequency scaling is reported but not controlled by the harness.
- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.
- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.
- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.
- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.
- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.
