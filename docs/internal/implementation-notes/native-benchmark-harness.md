# Native Benchmark Harness

Exact goal:
- Implement the native timing harness and statistical reporting.

Files created:
- `CMakeLists.txt`
- `src/main.cpp`
- `src/bench_timer.cpp`
- `src/stats.cpp`
- `src/report_json.cpp`
- `src/report_markdown.cpp`
- `include/qrs_bench/*.h`

Required behavior:
- Provide two modes:
  - `--quick`: small iteration count for smoke testing
  - `--full`: larger iteration count suitable for report generation
- Run warmup iterations before measurement.
- Time batches of operations, then report per-operation timing distributions.
- Record median, p95, p99, max, mean, stddev, and sample count.
- Use monotonic high-resolution clock.
- Prevent dead-code elimination by accumulating verification results into a volatile sink.
- Exit nonzero if harness-level argument parsing or file output fails.
- Backend correctness failures mark the backend unavailable with a reason in JSON unless the failure is outside backend setup.

Implementation constraints:
- Timed loops do not allocate memory intentionally.
- Key generation and signing happen outside timed loops.
- Messages/signatures are precomputed.
- The OpenSSL backend benchmarks both valid and invalid fixed-length signatures.
- Invalid fixed-length signatures are the same size as valid signatures and reach crypto verification, not structural rejection.

Acceptance commands:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/qrs_native_bench --quick --json out/quick.json --markdown out/quick.md
```

