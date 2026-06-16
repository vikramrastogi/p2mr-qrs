# Reproduction Matrix

Status: open public reproduction tracker.

The minimum public-pre-review target is independent reproduction on at least one
Apple Silicon macOS system and one Linux x86_64 system with OpenSSL 3.5 or newer
exposing `SLH-DSA-SHA2-128s`. The pre-activation target is broader: multiple
CPU vendors, multiple compilers, at least two reviewed verifier strategies, and
Bitcoin Core-shaped validation-path measurements.

| Platform class | CPU | OS | Compiler | OpenSSL/provider | Mode | Status | Artifact |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Apple Silicon macOS | Apple M2 Pro sample | Darwin arm64 | clang | OpenSSL 3.6.1/default | quick/standard/full sample | local sample only | `out/*.json`, `out/*.md` |
| Linux x86_64 | TBD | Linux x86_64 | GCC or clang | OpenSSL 3.5+ provider exposing SLH-DSA | quick and full | minimum public-pre-review target | reviewer attachment needed |
| Linux x86_64 alternate CPU | TBD | Linux x86_64 | GCC or clang | OpenSSL 3.5+ provider exposing SLH-DSA | full | pre-activation target | reviewer attachment needed |
| Second verifier backend | TBD | TBD | TBD | reviewed SLH-DSA backend | vector agreement and benchmark | pre-activation target | not wired |
| Bitcoin Core-shaped path | TBD | TBD | TBD | final verifier selection | validation-path benchmark | pre-activation target | not implemented |

## Required Reproduction Fields

Each reproduction should report:

- hardware and CPU model;
- OS and kernel version;
- compiler and compiler version;
- compiler flags;
- OpenSSL version and provider;
- SLH-DSA backend status;
- libsecp256k1 commit;
- benchmark mode;
- raw JSON and Markdown outputs;
- whether `bash scripts/release_check.sh` passed;
- whether `bash scripts/full_release_check.sh` passed.
