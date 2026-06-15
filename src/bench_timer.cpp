#include "qrs_bench/bench_timer.h"

#include <chrono>

namespace qrs {

volatile unsigned long long verification_sink = 0;

TimingSeries time_batches(const std::string& name,
                          const std::function<bool()>& fn,
                          std::size_t batches,
                          std::size_t iterations_per_batch,
                          std::size_t warmup_iterations) {
  for (std::size_t i = 0; i < warmup_iterations; ++i) {
    verification_sink += fn() ? 1ULL : 0ULL;
  }

  TimingSeries series;
  series.name = name;
  series.batches = batches;
  series.iterations_per_batch = iterations_per_batch;
  series.per_op_ns.reserve(batches);

  for (std::size_t batch = 0; batch < batches; ++batch) {
    const auto t0 = std::chrono::steady_clock::now();
    unsigned long long local = 0;
    for (std::size_t i = 0; i < iterations_per_batch; ++i) {
      local += fn() ? 1ULL : 0ULL;
    }
    const auto t1 = std::chrono::steady_clock::now();
    verification_sink += local;
    const auto elapsed_ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    series.per_op_ns.push_back(static_cast<double>(elapsed_ns) /
                               static_cast<double>(iterations_per_batch));
  }

  return series;
}

}  // namespace qrs

