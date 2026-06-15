#pragma once

#include "qrs_bench/stats.h"

#include <string>

namespace qrs {

struct SlhDsaResult {
  std::string status = "unavailable";
  std::string reason;
  std::string openssl_version;
  std::string provider = "unavailable";
  std::string algorithm = "SLH-DSA-SHA2-128s";
  std::string mode = "unavailable";
  std::size_t public_key_bytes = 0;
  std::size_t signature_bytes = 0;
  Stats valid_verify;
  Stats invalid_fixed_length_verify;
};

SlhDsaResult run_slh_dsa_benchmarks(bool quick);

}  // namespace qrs
