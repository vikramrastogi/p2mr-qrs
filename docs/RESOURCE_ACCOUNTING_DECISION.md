# Resource Accounting Decision

This document records how the current benchmark package maps evidence to the
QRS resource-accounting rule. It does not establish activation readiness by
itself.

## Decision criteria

The proposed no-additional-budget rule remains acceptable for Draft-stage
review only if the generated reports show all of the following on named
hardware:

- QRS valid p99 saturated-block estimate is below the individual Schnorr p99
  saturated-block estimate.
- QRS worst-observed invalid fixed-length p99 saturated-block estimate is below
  the individual Schnorr p99 saturated-block estimate.
- QRS valid p99 and worst-invalid p99 are below the experimental batch Schnorr
  p99 saturated-block estimate when that experimental baseline is available.
- Reviewed public batch Schnorr status is reported explicitly. If unavailable,
  the package must say so and must not synthesize a fake baseline from assumed
  speedups.

Decision language:

- If QRS valid and worst-invalid p99 are below individual Schnorr and
  experimental batch Schnorr p99 on named hardware, current evidence supports
  continuing with no additional QRS validation budget as the Draft-stage rule.
- If QRS exceeds experimental batch but not individual Schnorr, resource
  accounting remains unresolved for activation and requires reviewer decision.
- If QRS exceeds individual Schnorr, the draft must add an explicit per-QRS
  validation budget before activation.
- If a reviewed public batch Schnorr API is unavailable, experimental batch
  timings remain sensitivity analysis, not consensus proof.

## Current evidence

Source reports:

- `out/quick.md`
- `out/full.md`
- `out/batch-evidence.md`
- `out/resource-accounting-decision.md`
- `scripts/verify_qrs_vectors.py`
- `scripts/evaluate_resource_accounting.py`
- `scripts/release_check.sh`

The generated `out/resource-accounting-decision.*` files are the current
machine-readable decision artifacts. This document records the decision rule and
the committed sample full-run evidence; reviewers should regenerate `out/full.*`
using `docs/REPRODUCIBILITY.md` before citing local performance numbers.

Current sample run metadata from `out/full.json`:

- Hardware: Apple M2 Pro
- OS: Darwin 25.3.0 arm64
- Compiler: clang 17.0.0
- Build mode: Release/NDEBUG
- Compiler flags: CMake target `-O3 -Wall -Wextra -Wpedantic`
- OpenSSL: OpenSSL 3.6.1 27 Jan 2026, provider `default`
- SLH-DSA mode: EVP direct message signing/verification, no prehash digest,
  empty context string explicitly set
- libsecp256k1 commit:
  `aea86bc3509f3ceb5c047fe6567bf8910f72a728`
- Benchmark commit recorded by the sample report:
  `598148bf7bd0d4b491488868cead23f9ec7b7682`
- Working tree dirty status recorded by the sample report: `false`

Current full-report p99 saturated-block estimates:

| Estimate | p99 ms | Status |
| --- | ---: | --- |
| QRS valid saturated block | 92.037 | available |
| QRS worst-observed invalid fixed-length saturated block | 256.198 | available |
| Individual BIP-340 Schnorr saturated block | 399.126 | available |
| Experimental batch BIP-340 Schnorr saturated block | 242.702 | available, sensitivity analysis only |
| Reviewed public batch BIP-340 Schnorr saturated block | unavailable | no reviewed public API wired |

The worst observed invalid fixed-length SLH-DSA case in the sample run is
`spread_32_bitflips`. The invalid-fixed-length bucket is a crypto bucket: every case keeps
the 7,856-byte signature length and is asserted to fail through the SLH-DSA
verifier before timing, including wrong-message and wrong-public-key cases.

## Decision

Current full-run evidence leaves resource accounting unresolved for activation:
QRS is below individual Schnorr but exceeds the experimental batch Schnorr
sensitivity baseline on this benchmark package and machine. The generated
decision status is `activation_unresolved_batch_baseline`.

This conclusion is deliberately narrow:

- It does not establish activation readiness.
- It does not require an explicit per-QRS budget by itself, because the batch
  baseline is experimental and not reviewed consensus code.
- It does not establish cross-hardware or cross-implementation behavior.
- It does not replace Bitcoin Core validation-path integration.
- It does not treat the experimental batch Schnorr baseline as reviewed
  consensus code.
- It does not resolve the unavailable reviewed public batch Schnorr baseline.

If future native benchmarks, Bitcoin Core integration, implementation-variance
checks, or reviewed batch-Schnorr evidence fail the criteria above, the BIP must
add an explicit per-QRS validation budget before activation. The inactive
fallback design is specified in `docs/EXPLICIT_QRS_BUDGET_FALLBACK.md`.
