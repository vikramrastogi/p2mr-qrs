#pragma once

#include "qrs_bench/bench_timer.h"

#include <cstddef>
#include <string>
#include <vector>

namespace qrs {

struct Stats {
  bool available = false;
  std::string status = "unavailable";
  std::size_t samples = 0;
  std::size_t iterations_per_sample = 0;
  double mean_ns = 0.0;
  double stddev_ns = 0.0;
  double median_ns = 0.0;
  double p95_ns = 0.0;
  double p99_ns = 0.0;
  double max_ns = 0.0;
  double ops_per_sec = 0.0;
};

Stats summarize(const TimingSeries& series);
double percentile(std::vector<double> sorted_values, double p);

}  // namespace qrs

