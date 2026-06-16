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
- The experimental batch Schnorr estimate tightens the individual Schnorr
  baseline. If the experimental batch path is slower than individual Schnorr,
  it is still useful implementation evidence, but it does not test the
  batch-risk objection.
- A measured experimental batch implementation that is slower than individual
  Schnorr is not favorable evidence for QRS. In that case, the experimental
  batch result does not tighten the baseline and the resource-accounting
  question remains unresolved for activation. The decision record therefore
  separately reports hypothetical reviewed-batch speedups over individual
  Schnorr.
- Reviewed public batch Schnorr status is reported explicitly. If unavailable,
  the package must say so and must not synthesize a fake baseline from assumed
  speedups.

Decision language:

- If QRS valid and worst-invalid p99 are below individual Schnorr and below an
  experimental batch Schnorr p99 that tightens the individual baseline on named
  hardware, the generated status is
  `supports_no_additional_budget_for_draft_review`.
- If QRS exceeds experimental batch but not individual Schnorr, resource
  accounting remains unresolved for activation and the generated status is
  `unresolved_requires_reviewer_decision`.
- If the experimental batch baseline is available but slower than individual
  Schnorr, resource accounting also remains unresolved for activation, because
  that run did not exercise the stricter batch-baseline comparison.
- If QRS exceeds individual Schnorr, native data is missing, invalid
  fixed-length data is missing, or required report metadata is incomplete, the
  generated status is `explicit_budget_required_before_activation`.
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
- Compiler: clang 17.0.0 (clang-1700.6.4.2)
- Build mode: Release/NDEBUG (`cmake_build_type = Release`)
- Compiler flags: `-O3 -Wall -Wextra -Wpedantic`
- OpenSSL: OpenSSL 3.6.1 27 Jan 2026, provider `default`
- SLH-DSA mode: EVP direct message signing/verification, no prehash digest,
  empty context string explicitly set
- libsecp256k1 commit:
  `aea86bc3509f3ceb5c047fe6567bf8910f72a728`
- Benchmark commit recorded by the sample report:
  `3821510b6bcc223e0d716dc65021f249afca3e68`
- Working tree dirty status recorded by the sample report: `false`

Current full-report p99 saturated-block estimates:

| Estimate | p99 ms | Status |
| --- | ---: | --- |
| QRS valid saturated block | 90.887 | available |
| QRS worst-observed invalid fixed-length saturated block | 95.910 | available |
| Individual BIP-340 Schnorr saturated block | 345.599 | available |
| Experimental batch BIP-340 Schnorr saturated block | 240.083 | available, sensitivity analysis only |
| Reviewed public batch BIP-340 Schnorr saturated block | unavailable | no reviewed public API wired |

The generated decision also includes a `batch_sensitivity` object. It reports
the experimental batch speedup relative to individual Schnorr, the speedup at
which QRS valid and worst-invalid p99 would exceed a hypothetical batch
baseline, and several illustrative speedup rows. Those rows are sensitivity
analysis only. They are not measured baselines and are not used as fake batch
evidence.

The generated decision also includes `depth_sensitivity` with
`qrs_depth0_p99_ms`, `qrs_depth128_p99_ms`, and `qrs_depth_binding_case`.
Depth-sensitive rows count Merkle path witness weight and TapBranch hash
counts. They do not benchmark Bitcoin Core branch-hash implementation overhead;
that remains part of the Core validation-path integration blocker.

The generated decision also includes explicit fallback trigger checks. A
fallback trigger is not an activation rule by itself; it is a flag that the BIP
must either add an explicit QRS validation budget before activation or remain
unresolved. Current trigger checks include individual Schnorr, a 2.5x hypothetical reviewed-batch-Schnorr baseline, a 3.0x hypothetical reviewed-batch-Schnorr baseline, any reviewed public batch-Schnorr implementation becomes available, and Bitcoin Core validation-path integration overhead.

The worst observed invalid fixed-length SLH-DSA case in the sample run is
`wrong_message`. The invalid-fixed-length bucket is a crypto bucket: every case keeps
the 7,856-byte signature length and is asserted to fail through the SLH-DSA
verifier before timing, including wrong-message and wrong-public-key cases.
Verifier-cost review still requires algorithmic worst-case analysis; see
`SLH_DSA_VERIFY_COST_ANALYSIS.md`.

## Decision

Current full-run evidence supports continuing to evaluate the no-additional-budget
rule as Draft-stage review material only when the regenerated experimental batch
baseline is both available and stricter than individual Schnorr on the tested
machine. If a regenerated report shows experimental batch Schnorr slower than
individual Schnorr, the generated decision status becomes
`unresolved_requires_reviewer_decision` even if QRS remains below individual
Schnorr.

This conclusion is deliberately narrow:

- It does not establish activation readiness.
- It does not prove that no explicit per-QRS budget will be required before
  activation.
- It does not establish cross-hardware or cross-implementation behavior.
- It does not replace Bitcoin Core validation-path integration.
- It does not treat the experimental batch Schnorr baseline as reviewed
  consensus code.
- It does not resolve the unavailable reviewed public batch Schnorr baseline.

If future native benchmarks, Bitcoin Core integration, implementation-variance
checks, or reviewed batch-Schnorr evidence fail the criteria above, the BIP must
add an explicit per-QRS validation budget before activation. The inactive
fallback design is specified in `docs/EXPLICIT_QRS_BUDGET_FALLBACK.md`.
