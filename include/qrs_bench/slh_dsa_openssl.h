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

struct SlhDsaBackendInfo {
  std::string name;
  std::string status = "unavailable";
  std::string reason;
  std::string provider = "unavailable";
  std::string version = "unavailable";
  std::size_t public_key_bytes = 32;
  std::size_t signature_bytes = 7856;
};

struct SlhDsaBackendProbe {
  std::string keygen = "unavailable";
  std::string sign = "unavailable";
  std::string verify_valid = "unavailable";
  std::string verify_mutated_signature = "unavailable";
  std::size_t public_key_bytes = 32;
  std::size_t signature_bytes = 7856;
  std::string context_string = "unavailable";
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
  SlhDsaBackendProbe backend_probe;
  Stats valid_verify;
  Stats invalid_fixed_length_verify;
  std::vector<NamedStats> invalid_fixed_length_cases;
  Stats invalid_fixed_length_min_observed;
  Stats invalid_fixed_length_best_observed;
  Stats invalid_fixed_length_median_observed;
  Stats invalid_fixed_length_p99_observed;
  Stats invalid_fixed_length_worst_observed;
  std::string invalid_fixed_length_case_name_worst;
  std::vector<SlhDsaBackendInfo> backends;
};

SlhDsaResult run_slh_dsa_benchmarks(BenchmarkMode mode);

}  // namespace qrs
