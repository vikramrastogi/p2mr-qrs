#pragma once

#include "qrs_bench/benchmark_mode.h"
#include "qrs_bench/stats.h"

#include <string>
#include <vector>

namespace qrs {

struct NamedStats {
  std::string name;
  Stats stats;
};

struct SlhDsaResult {
  std::string status = "unavailable";
  std::string reason;
  std::string openssl_version;
  std::string provider = "unavailable";
  std::string algorithm = "SLH-DSA-SHA2-128s";
  std::string mode = "unavailable";
  std::size_t public_key_bytes = 32;
  std::size_t signature_bytes = 7856;
  Stats valid_verify;
  Stats invalid_fixed_length_verify;
  std::vector<NamedStats> invalid_fixed_length_cases;
  Stats invalid_fixed_length_min_observed;
  Stats invalid_fixed_length_best_observed;
  Stats invalid_fixed_length_median_observed;
  Stats invalid_fixed_length_p99_observed;
  Stats invalid_fixed_length_worst_observed;
  std::string invalid_fixed_length_case_name_worst;
};

SlhDsaResult run_slh_dsa_benchmarks(BenchmarkMode mode);

}  // namespace qrs
