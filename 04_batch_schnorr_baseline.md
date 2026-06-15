# Batch Schnorr Baseline

Exact goal:
- Add an experimental native BIP-340 batch Schnorr baseline for sensitivity
  analysis.
- Keep reviewed/public libsecp256k1 batch Schnorr status separate and report
  precisely why it is unavailable.

Files to inspect:
- Bundled libsecp256k1 public Schnorr APIs.
- Bundled libsecp256k1 Schnorr tests and benchmark sources.
- Upstream `bitcoin-core/secp256k1` heads matching batch/Schnorr/BIP-340
  review candidates.

Current status:
- The bundled upstream libsecp256k1 commit `aea86bc3509f3ceb5c047fe6567bf8910f72a728` does not expose a reviewed public batch Schnorr verification API. The public Schnorr header exposes `secp256k1_schnorrsig_verify`, and local upstream tests still refer to `verify_batch (TODO)`.
- `git ls-remote --heads https://github.com/bitcoin-core/secp256k1.git '*batch*' '*schnorr*' '*bip340*'` returned no matching upstream heads, so this package does not have a reviewed experimental branch to label and wire as reviewed.
- The benchmark package implements an experimental native BIP-340 batch verifier using libsecp256k1 internals, deterministic nonzero coefficients derived from a domain-separated hash of all batch inputs, and `a_0 = 1`.
- The experimental verifier is not a reviewed public libsecp256k1 API and must be treated as sensitivity analysis only.
- Experimental batch timing includes signature/public-key parsing, BIP-340 challenge computation, coefficient derivation, and the multiscalar batch equation check.
- The JSON report emits `.benchmarks.schnorr_bip340.batch_reviewed_public_api_status = "unavailable"` and keeps `.benchmarks.schnorr_bip340.batch.status = "unavailable"` as the reviewed-public alias.
- The JSON report emits `.benchmarks.schnorr_bip340.batch_experimental_valid.status = "available"` and `.benchmarks.schnorr_bip340.batch_experimental_invalid.status = "available"` when the local experimental verifier passes correctness assertions.
- The benchmark must never silently omit this baseline.
- `scripts/check_batch_schnorr_baseline.py` reproduces this evidence and writes `out/batch-evidence.json` plus `out/batch-evidence.md`. If the upstream query fails or exceeds `--upstream-timeout-seconds` (default 15), the script reports `evidence-incomplete` and exits nonzero.

Required behavior:
- If a reviewed batch API is available, benchmark it.
- If only an experimental batch branch or local experimental implementation is available, benchmark it separately and label it experimental.
- If no acceptable batch API is available, emit a JSON field showing batch baseline unavailable and explain why.
- Never silently omit the batch baseline.

Implementation constraints:
- Batch sizes must include realistic block-shaped sizes:
  - 64
  - 128
  - 512
  - 2048
  - saturated-by-weight count
- Report batch verification as per-signature cost and per-block cost.
- Include invalid-batch behavior. The current experimental verifier rejects the batch without timing any fallback to individual verification.

Acceptance commands:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
python3 scripts/check_batch_schnorr_baseline.py --json out/batch-evidence.json --markdown out/batch-evidence.md --upstream-timeout-seconds 15
./build/qrs_native_bench --quick --json out/quick.json --markdown out/quick.md
python3 scripts/assert_quick_report.py --json out/quick.json --markdown out/quick.md --batch-evidence-json out/batch-evidence.json
jq '.benchmarks.schnorr_bip340.batch_experimental_valid.status' out/quick.json
jq '.benchmarks.schnorr_bip340.batch_reviewed_public_api_status' out/quick.json
grep -n "experimental native BIP-340 batch baseline" out/quick.md
grep -n "not a reviewed public libsecp256k1 API" out/quick.md
grep -n "QRS vs experimental batch Schnorr" out/quick.md
grep -n "reviewed public batch" out/quick.md
```
