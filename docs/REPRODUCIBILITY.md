# Reproducibility Protocol

This repository's local reports are not independent consensus evidence. A
reviewer rerun should produce raw JSON and Markdown artifacts that can be
compared mechanically, while still recognizing that timing results are
machine-specific.

The platform target table is maintained in `REPRODUCTION_MATRIX.md`. The
SLH-DSA verifier-cost caveat is maintained in
`SLH_DSA_VERIFY_COST_ANALYSIS.md`.

## Minimum Toolchain

- CMake 3.22 or newer.
- A C++17 compiler with release optimization support.
- Python 3.10 or newer.
- `jq` for release-check assertions.
- OpenSSL 3.5 or newer with OpenSSL SLH-DSA support exposed by a loaded
  provider through EVP.
- The vendored `third_party/secp256k1/` source pinned by
  `third_party/secp256k1.COMMIT`.

Strict QRS vector and release checks require `SLH-DSA-SHA2-128s` to be
available through `EVP_PKEY_CTX_new_from_name(..., "SLH-DSA-SHA2-128s", ...)`.
The committed sample reports were generated with OpenSSL 3.6.1 from the
`default` provider. If that EVP lookup fails locally, report the OpenSSL
version, provider configuration, and error output; the run is useful as an
environment diagnostic but is not a strict reproduction.

After building, a non-secret backend probe is available:

```sh
./build/qrs_native_bench --slh-probe
```

The probe prints OpenSSL version/provider metadata and confirms keygen, signing,
valid verification, mutated fixed-length signature rejection, 32-byte public-key
size, 7,856-byte signature size, and explicit empty-context configuration. It
does not print generated private key material.

CI structural checks run the build, provisional fixture checks, digest
agreement, quick benchmark, and advisory resource-accounting evaluation. They
are portability smoke checks when OpenSSL SLH-DSA is unavailable. They do not
replace `bash scripts/release_check.sh` on a machine with OpenSSL 3.5+ exposing
`SLH-DSA-SHA2-128s`.

## Benchmark Modes

- `--quick`: release-check and CI smoke mode. It proves the pipeline and
  produces advisory resource-accounting output.
- `--standard`: medium local rerun for sanity checking.
- `--full`: reviewer-facing timing run. Use this for numbers cited in public
  review threads.

All timing tables report the median of per-batch means, p95 of per-batch means,
p99 of per-batch means, and maximum of per-batch means. They are not raw
individual-operation latency samples.

## Experimental Batch Baseline

The reviewed public batch Schnorr baseline is distinct from the experimental
batch baseline. The package records reviewed-public batch availability through
`scripts/check_batch_schnorr_baseline.py`. The experimental batch benchmark is
native sensitivity analysis only; it is not a reviewed public libsecp256k1 API
and is not consensus-grade validation code.

`scripts/evaluate_resource_accounting.py` also emits batch-speedup sensitivity
rows. Those rows divide the measured individual Schnorr saturated-block
estimate by illustrative speedups to show where QRS would become unresolved;
they are not measured baselines and must not be cited as batch evidence.

Ordinary local release checks use local-only batch evidence. When publishing a
network-checked upstream batch evidence refresh, run:

```sh
QRS_RELEASE_NETWORK_BATCH=1 bash scripts/release_check.sh
```

## Required Commands

For the strict local release contract:

```sh
bash scripts/release_check.sh
```

The release gate writes fresh reports under `build/release-check/` by default
and validates those fresh artifacts. Checked-in `out/` files are sample
artifacts. Use `QRS_RELEASE_UPDATE_ARTIFACTS=1 bash scripts/release_check.sh`
only when intentionally refreshing committed samples; the gate fails if the
refresh leaves uncommitted `out/` changes.

For a full reviewer timing rerun:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
python3 scripts/check_batch_schnorr_baseline.py \
  --json out/batch-evidence.json \
  --markdown out/batch-evidence.md
python3 scripts/verify_qrs_vectors.py test_vectors/ \
  --binary build/qrs_native_bench \
  --require-crypto
./build/qrs_native_bench --full \
  --json out/full.json \
  --markdown out/full.md
python3 scripts/assert_quick_report.py \
  --json out/full.json \
  --markdown out/full.md \
  --batch-evidence-json out/batch-evidence.json
python3 scripts/evaluate_resource_accounting.py \
  --report-json out/full.json \
  --batch-evidence-json out/batch-evidence.json \
  --json out/resource-accounting-decision.json \
  --markdown out/resource-accounting-decision.md
```

## Expected Outputs

- `out/quick.json` and `out/quick.md` from release checks.
- `out/full.json` and `out/full.md` from reviewer timing runs.
- `out/batch-evidence.json` and `out/batch-evidence.md`.
- `out/resource-accounting-decision.json`.
- `out/resource-accounting-decision.md`.

## Required Metadata

The generated report must identify:

- hardware and CPU model;
- operating system;
- compiler and compiler version;
- compiler flags;
- CMake build type;
- benchmark binary build mode;
- OpenSSL version and provider;
- OpenSSL SLH-DSA provider availability;
- SLH-DSA mode;
- libsecp256k1 commit;
- benchmark harness commit;
- working-tree dirty status;
- benchmark schema.

## Reporting Reproduction

Use the benchmark reproduction issue template and attach the raw JSON and
Markdown artifacts. Include:

- OS;
- CPU;
- compiler;
- compiler flags;
- OpenSSL version/provider;
- libsecp256k1 commit;
- benchmark mode (`quick`, `standard`, or `full`);
- whether `bash scripts/release_check.sh` passed;
- batch status;
- JSON attachment;
- Markdown attachment.

Independent reproduction can strengthen or falsify Draft-stage resource
accounting. It cannot by itself establish activation readiness unless the
remaining activation blockers are also closed: final BIP-360/QRS definitions,
Bitcoin Core validation-path integration, reviewed implementation selection,
and reviewer agreement on the batch-Schnorr baseline.
