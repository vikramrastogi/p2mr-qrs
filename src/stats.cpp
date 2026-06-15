#include "qrs_bench/stats.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace qrs {

double percentile(std::vector<double> sorted_values, double p) {
  if (sorted_values.empty()) {
    return 0.0;
  }
  std::sort(sorted_values.begin(), sorted_values.end());
  if (sorted_values.size() == 1) {
    return sorted_values.front();
  }
  const double rank = (p / 100.0) * static_cast<double>(sorted_values.size() - 1);
  const auto lo = static_cast<std::size_t>(std::floor(rank));
  const auto hi = static_cast<std::size_t>(std::ceil(rank));
  const double frac = rank - static_cast<double>(lo);
  return sorted_values[lo] * (1.0 - frac) + sorted_values[hi] * frac;
}

Stats summarize(const TimingSeries& series) {
  Stats s;
  s.available = true;
  s.status = "available";
  s.samples = series.per_op_ns.size();
  s.iterations_per_sample = series.iterations_per_batch;
  if (series.per_op_ns.empty()) {
    s.available = false;
    s.status = "unavailable";
    return s;
  }

  const double sum =
      std::accumulate(series.per_op_ns.begin(), series.per_op_ns.end(), 0.0);
  s.mean_ns = sum / static_cast<double>(series.per_op_ns.size());
  double accum = 0.0;
  for (double v : series.per_op_ns) {
    const double d = v - s.mean_ns;
    accum += d * d;
  }
  s.stddev_ns = std::sqrt(accum / static_cast<double>(series.per_op_ns.size()));
  s.median_ns = percentile(series.per_op_ns, 50.0);
  s.p95_ns = percentile(series.per_op_ns, 95.0);
  s.p99_ns = percentile(series.per_op_ns, 99.0);
  s.max_ns = *std::max_element(series.per_op_ns.begin(), series.per_op_ns.end());
  s.ops_per_sec = s.mean_ns > 0.0 ? 1'000'000'000.0 / s.mean_ns : 0.0;
  return s;
}

}  // namespace qrs

