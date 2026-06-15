#pragma once

#include "qrs_bench/stats.h"

#include <cstddef>
#include <string>
#include <vector>

namespace qrs {

struct BatchBenchmark {
  std::size_t batch_size = 0;
  Stats total_verify;
  Stats per_signature_verify;
};

struct SchnorrResult {
  std::string status = "unavailable";
  std::string reason;
  std::string libsecp256k1_commit = "unavailable";
  std::string compile_flags = "unavailable";
  Stats individual_valid_verify;
  Stats individual_invalid_verify;
  std::string batch_reviewed_public_api_status = "unavailable";
  std::string batch_reviewed_public_api_reason;
  Stats batch_reviewed_public_per_signature_verify;
  std::string batch_experimental_status = "unavailable";
  std::string batch_experimental_reason;
  std::string batch_experimental_label =
      "Experimental native BIP-340 batch baseline; not a reviewed public libsecp256k1 API.";
  std::string batch_experimental_coefficient_policy =
      "Deterministic nonzero coefficients derived from a domain-separated hash of all batch inputs; a_0 = 1.";
  std::string batch_experimental_invalid_policy =
      "Invalid batch rejects as a batch; no fallback to individual verification is timed.";
  std::string batch_experimental_timing_scope =
      "Timed verification includes signature/public-key parsing, BIP-340 challenge computation, coefficient derivation, and the multiscalar batch equation check.";
  std::vector<BatchBenchmark> batch_experimental_valid;
  std::vector<BatchBenchmark> batch_experimental_invalid;
  Stats batch_experimental_primary_per_signature_verify;
};

SchnorrResult run_schnorr_benchmarks(bool quick);

}  // namespace qrs
