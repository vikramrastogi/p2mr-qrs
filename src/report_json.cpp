#include "qrs_bench/report_json.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace qrs {

namespace {

std::string esc(const std::string& s) {
  std::ostringstream o;
  for (const char c : s) {
    switch (c) {
      case '\\': o << "\\\\"; break;
      case '"': o << "\\\""; break;
      case '\n': o << "\\n"; break;
      case '\r': o << "\\r"; break;
      case '\t': o << "\\t"; break;
      default: o << c; break;
    }
  }
  return o.str();
}

void json_stats(std::ostringstream& o, const Stats& s, int indent) {
  const std::string pad(indent, ' ');
  o << "{\n";
  o << pad << "  \"status\": \"" << esc(s.status) << "\",\n";
  if (!s.available) {
    o << pad << "  \"samples\": null,\n";
    o << pad << "  \"median_ns\": null,\n";
    o << pad << "  \"p95_ns\": null,\n";
    o << pad << "  \"p99_ns\": null,\n";
    o << pad << "  \"max_ns\": null\n";
  } else {
    o << pad << "  \"samples\": " << s.samples << ",\n";
    o << pad << "  \"iterations_per_sample\": " << s.iterations_per_sample << ",\n";
    o << pad << "  \"mean_ns\": " << s.mean_ns << ",\n";
    o << pad << "  \"stddev_ns\": " << s.stddev_ns << ",\n";
    o << pad << "  \"median_ns\": " << s.median_ns << ",\n";
    o << pad << "  \"p95_ns\": " << s.p95_ns << ",\n";
    o << pad << "  \"p99_ns\": " << s.p99_ns << ",\n";
    o << pad << "  \"max_ns\": " << s.max_ns << ",\n";
    o << pad << "  \"ops_per_sec\": " << s.ops_per_sec << "\n";
  }
  o << pad << "}";
}

void json_estimate(std::ostringstream& o, const BlockEstimate& e, int indent) {
  const std::string pad(indent, ' ');
  o << "{\n";
  o << pad << "  \"status\": \"" << e.status << "\",\n";
  o << pad << "  \"qrs_merkle_depth\": " << e.qrs_merkle_depth << ",\n";
  o << pad << "  \"merkle_path_hashes_per_input\": "
    << e.merkle_path_hashes_per_input << ",\n";
  o << pad << "  \"merkle_path_hashes_per_block\": "
    << e.merkle_path_hashes_per_block << ",\n";
  o << pad << "  \"input_weight_wu\": " << e.input_weight_wu << ",\n";
  o << pad << "  \"max_inputs\": " << e.max_inputs << ",\n";
  if (!e.available) {
    o << pad << "  \"median_ms\": null,\n";
    o << pad << "  \"p95_ms\": null,\n";
    o << pad << "  \"p99_ms\": null,\n";
    o << pad << "  \"max_ms\": null\n";
  } else {
    o << pad << "  \"median_ms\": " << e.ms.at("median_ms") << ",\n";
    o << pad << "  \"p95_ms\": " << e.ms.at("p95_ms") << ",\n";
    o << pad << "  \"p99_ms\": " << e.ms.at("p99_ms") << ",\n";
    o << pad << "  \"max_ms\": " << e.ms.at("max_ms") << "\n";
  }
  o << pad << "}";
}

void json_estimate_vector(std::ostringstream& o,
                          const std::vector<BlockEstimate>& estimates,
                          int indent) {
  const std::string pad(indent, ' ');
  o << "[\n";
  for (std::size_t i = 0; i < estimates.size(); ++i) {
    const auto& e = estimates[i];
    o << pad << "  ";
    json_estimate(o, e, indent + 2);
    o << (i + 1 == estimates.size() ? "\n" : ",\n");
  }
  o << pad << "]";
}

void json_batch_benchmarks(std::ostringstream& o,
                           const std::vector<BatchBenchmark>& batches,
                           int indent) {
  const std::string pad(indent, ' ');
  o << "[\n";
  for (std::size_t i = 0; i < batches.size(); ++i) {
    const auto& b = batches[i];
    o << pad << "  {\n";
    o << pad << "    \"batch_size\": " << b.batch_size << ",\n";
    o << pad << "    \"total_verify\": ";
    json_stats(o, b.total_verify, indent + 4);
    o << ",\n";
    o << pad << "    \"per_signature_verify\": ";
    json_stats(o, b.per_signature_verify, indent + 4);
    o << "\n";
    o << pad << "  }" << (i + 1 == batches.size() ? "\n" : ",\n");
  }
  o << pad << "]";
}

void json_named_stats(std::ostringstream& o, const std::vector<NamedStats>& named, int indent) {
  const std::string pad(indent, ' ');
  o << "{\n";
  for (std::size_t i = 0; i < named.size(); ++i) {
    o << pad << "  \"" << esc(named[i].name) << "\": ";
    json_stats(o, named[i].stats, indent + 2);
    o << (i + 1 == named.size() ? "\n" : ",\n");
  }
  o << pad << "}";
}

void json_slh_backends(std::ostringstream& o,
                       const std::vector<SlhDsaBackendInfo>& backends,
                       int indent) {
  const std::string pad(indent, ' ');
  o << "{\n";
  for (std::size_t i = 0; i < backends.size(); ++i) {
    const auto& b = backends[i];
    o << pad << "  \"" << esc(b.name) << "\": {\n";
    o << pad << "    \"status\": \"" << esc(b.status) << "\",\n";
    o << pad << "    \"reason\": \"" << esc(b.reason) << "\",\n";
    o << pad << "    \"provider\": \"" << esc(b.provider) << "\",\n";
    o << pad << "    \"version\": \"" << esc(b.version) << "\",\n";
    o << pad << "    \"public_key_bytes\": " << b.public_key_bytes << ",\n";
    o << pad << "    \"signature_bytes\": " << b.signature_bytes << "\n";
    o << pad << "  }" << (i + 1 == backends.size() ? "\n" : ",\n");
  }
  o << pad << "}";
}

}  // namespace

std::string render_json(const Environment& env,
                        const SlhDsaResult& slh,
                        const SchnorrResult& schnorr,
                        const QrsValidationPathResult& qrs_path,
                        const BlockModel& block_model,
                        const std::string& mode) {
  std::ostringstream o;
  o << std::fixed << std::setprecision(3);
  o << "{\n";
  o << "  \"schema\": \"qrs-native-bench/v0\",\n";
  o << "  \"mode\": \"" << esc(mode) << "\",\n";
  o << "  \"environment\": {\n";
  std::size_t idx = 0;
  for (const auto& [k, v] : env.fields) {
    o << "    \"" << esc(k) << "\": \"" << esc(v) << "\"";
    o << (++idx == env.fields.size() ? "\n" : ",\n");
  }
  o << "  },\n";
  o << "  \"benchmarks\": {\n";
  o << "    \"slh_dsa_sha2_128s\": {\n";
  o << "      \"status\": \"" << esc(slh.status) << "\",\n";
  o << "      \"reason\": \"" << esc(slh.reason) << "\",\n";
  o << "      \"openssl_version\": \"" << esc(slh.openssl_version) << "\",\n";
  o << "      \"provider\": \"" << esc(slh.provider) << "\",\n";
  o << "      \"algorithm\": \"" << esc(slh.algorithm) << "\",\n";
  o << "      \"mode\": \"" << esc(slh.mode) << "\",\n";
  o << "      \"public_key_bytes\": " << slh.public_key_bytes << ",\n";
  o << "      \"signature_bytes\": " << slh.signature_bytes << ",\n";
  o << "      \"backends\": ";
  json_slh_backends(o, slh.backends, 6);
  o << ",\n";
  o << "      \"valid_verify\": ";
  json_stats(o, slh.valid_verify, 6);
  o << ",\n      \"invalid_fixed_length_verify\": ";
  json_stats(o, slh.invalid_fixed_length_verify, 6);
  o << ",\n      \"invalid_fixed_length_cases\": ";
  json_named_stats(o, slh.invalid_fixed_length_cases, 6);
  o << ",\n      \"invalid_fixed_length_min_observed\": ";
  json_stats(o, slh.invalid_fixed_length_min_observed, 6);
  o << ",\n      \"invalid_fixed_length_best_observed\": ";
  json_stats(o, slh.invalid_fixed_length_best_observed, 6);
  o << ",\n      \"invalid_fixed_length_median_observed\": ";
  json_stats(o, slh.invalid_fixed_length_median_observed, 6);
  o << ",\n      \"invalid_fixed_length_p99_observed\": ";
  json_stats(o, slh.invalid_fixed_length_p99_observed, 6);
  o << ",\n      \"invalid_fixed_length_worst_observed\": ";
  json_stats(o, slh.invalid_fixed_length_worst_observed, 6);
  o << ",\n      \"invalid_fixed_length_case_name_worst\": \""
    << esc(slh.invalid_fixed_length_case_name_worst) << "\"";
  o << "\n    },\n";
  o << "    \"schnorr_bip340\": {\n";
  o << "      \"status\": \"" << esc(schnorr.status) << "\",\n";
  o << "      \"reason\": \"" << esc(schnorr.reason) << "\",\n";
  o << "      \"libsecp256k1_commit\": \"" << esc(schnorr.libsecp256k1_commit) << "\",\n";
  o << "      \"compile_flags\": \"" << esc(schnorr.compile_flags) << "\",\n";
  o << "      \"individual_valid_verify\": ";
  json_stats(o, schnorr.individual_valid_verify, 6);
  o << ",\n      \"individual_invalid_verify\": ";
  json_stats(o, schnorr.individual_invalid_verify, 6);
  o << ",\n      \"batch_reviewed_public_api_status\": \""
    << esc(schnorr.batch_reviewed_public_api_status) << "\",\n";
  o << "      \"batch_reviewed_public_api_reason\": \""
    << esc(schnorr.batch_reviewed_public_api_reason) << "\",\n";
  o << "      \"batch_reviewed_public_api\": {\n";
  o << "        \"status\": \"" << esc(schnorr.batch_reviewed_public_api_status) << "\",\n";
  o << "        \"reason\": \"" << esc(schnorr.batch_reviewed_public_api_reason) << "\"\n";
  o << "      },\n";
  o << "      \"batch\": {\n";
  o << "        \"status\": \"" << esc(schnorr.batch_reviewed_public_api_status) << "\",\n";
  o << "        \"reason\": \"" << esc(schnorr.batch_reviewed_public_api_reason) << "\",\n";
  o << "        \"per_signature_verify\": ";
  json_stats(o, schnorr.batch_reviewed_public_per_signature_verify, 8);
  o << "\n      }\n";
  o << ",\n      \"batch_experimental\": {\n";
  o << "        \"status\": \"" << esc(schnorr.batch_experimental_status) << "\",\n";
  o << "        \"reason\": \"" << esc(schnorr.batch_experimental_reason) << "\",\n";
  o << "        \"label\": \"" << esc(schnorr.batch_experimental_label) << "\"\n";
  o << "      }";
  o << ",\n      \"batch_experimental_challenge_self_test\": {\n";
  o << "        \"status\": \""
    << esc(schnorr.batch_experimental_challenge_self_test_status) << "\",\n";
  o << "        \"result\": \""
    << (schnorr.batch_experimental_challenge_self_test_status == "available" ? "passed"
                                                                             : "unavailable")
    << "\",\n";
  o << "        \"description\": \"Compared hardcoded-midstate challenge computation against independent BIP0340/challenge tagged-hash computation.\",\n";
  o << "        \"reason\": \""
    << esc(schnorr.batch_experimental_challenge_self_test_reason) << "\"\n";
  o << "      }";
  o << ",\n      \"batch_experimental_valid\": {\n";
  o << "        \"status\": \"" << esc(schnorr.batch_experimental_status) << "\",\n";
  o << "        \"reason\": \"" << esc(schnorr.batch_experimental_reason) << "\",\n";
  o << "        \"label\": \"" << esc(schnorr.batch_experimental_label) << "\",\n";
  o << "        \"challenge_self_test_status\": \""
    << esc(schnorr.batch_experimental_challenge_self_test_status) << "\",\n";
  o << "        \"challenge_self_test_reason\": \""
    << esc(schnorr.batch_experimental_challenge_self_test_reason) << "\",\n";
  o << "        \"coefficient_policy\": \""
    << esc(schnorr.batch_experimental_coefficient_policy) << "\",\n";
  o << "        \"timing_scope\": \""
    << esc(schnorr.batch_experimental_timing_scope) << "\",\n";
  o << "        \"primary_per_signature_verify\": ";
  json_stats(o, schnorr.batch_experimental_primary_per_signature_verify, 8);
  o << ",\n        \"batch_sizes\": ";
  json_batch_benchmarks(o, schnorr.batch_experimental_valid, 8);
  o << "\n      },\n";
  o << "      \"batch_experimental_invalid\": {\n";
  o << "        \"status\": \"" << esc(schnorr.batch_experimental_status) << "\",\n";
  o << "        \"reason\": \"" << esc(schnorr.batch_experimental_reason) << "\",\n";
  o << "        \"label\": \"" << esc(schnorr.batch_experimental_label) << "\",\n";
  o << "        \"challenge_self_test_status\": \""
    << esc(schnorr.batch_experimental_challenge_self_test_status) << "\",\n";
  o << "        \"challenge_self_test_reason\": \""
    << esc(schnorr.batch_experimental_challenge_self_test_reason) << "\",\n";
  o << "        \"invalid_batch_policy\": \""
    << esc(schnorr.batch_experimental_invalid_policy) << "\",\n";
  o << "        \"timing_scope\": \""
    << esc(schnorr.batch_experimental_timing_scope) << "\",\n";
  o << "        \"batch_sizes\": ";
  json_batch_benchmarks(o, schnorr.batch_experimental_invalid, 8);
  o << "\n      }\n";
  o << "    },\n";
  o << "    \"qrs_validation_path\": {\n";
  o << "      \"status\": \"" << esc(qrs_path.status) << "\",\n";
  o << "      \"reason\": \"" << esc(qrs_path.reason) << "\",\n";
  o << "      \"scope\": \"" << esc(qrs_path.scope) << "\",\n";
  o << "      \"qrs_msg_scope\": \"" << esc(qrs_path.qrs_msg_scope) << "\",\n";
  o << "      \"structural_checks\": ";
  json_stats(o, qrs_path.structural_checks, 6);
  o << ",\n      \"qrs_msg_construction\": ";
  json_stats(o, qrs_path.qrs_msg_construction, 6);
  o << ",\n      \"verifier_valid\": ";
  json_stats(o, qrs_path.verifier_valid, 6);
  o << ",\n      \"total_valid\": ";
  json_stats(o, qrs_path.total_valid, 6);
  o << ",\n      \"invalid_structural\": ";
  json_stats(o, qrs_path.invalid_structural, 6);
  o << ",\n      \"invalid_fixed_length\": ";
  json_stats(o, qrs_path.invalid_fixed_length, 6);
  o << "\n    }\n";
  o << "  },\n";
  o << "  \"block_model\": {\n";
  o << "    \"assumption\": \"simplified per-input saturation approximation; real blocks include transaction overhead and outputs\",\n";
  o << "    \"block_weight_wu\": " << block_model.block_weight_wu << ",\n";
  o << "    \"p2tr_keypath_input_wu\": " << block_model.p2tr_keypath_input_wu << ",\n";
  o << "    \"p2mr_qrs_depth0_input_wu\": " << block_model.p2mr_qrs_depth0_input_wu << ",\n";
  o << "    \"p2mr_qrs_control_path_wu_per_depth\": "
    << block_model.p2mr_qrs_control_path_wu_per_depth << ",\n";
  o << "    \"qrs_saturated_block_valid\": ";
  json_estimate(o, block_model.qrs_saturated_block_valid, 4);
  o << ",\n    \"qrs_saturated_block_invalid_fixed_length\": ";
  json_estimate(o, block_model.qrs_saturated_block_invalid_fixed_length, 4);
  o << ",\n    \"qrs_saturated_block_valid_by_depth\": ";
  json_estimate_vector(o, block_model.qrs_saturated_block_valid_by_depth, 4);
  o << ",\n    \"qrs_saturated_block_invalid_fixed_length_by_depth\": ";
  json_estimate_vector(o, block_model.qrs_saturated_block_invalid_fixed_length_by_depth, 4);
  o << ",\n    \"schnorr_individual_saturated_block\": ";
  json_estimate(o, block_model.schnorr_individual_saturated_block, 4);
  o << ",\n    \"schnorr_experimental_batch_saturated_block\": ";
  json_estimate(o, block_model.schnorr_experimental_batch_saturated_block, 4);
  o << ",\n    \"schnorr_batch_saturated_block\": ";
  json_estimate(o, block_model.schnorr_batch_saturated_block, 4);
  o << "\n  }\n";
  o << "}\n";
  return o.str();
}

void write_text_file(const std::string& path, const std::string& text) {
  const std::filesystem::path p(path);
  if (p.has_parent_path()) {
    std::filesystem::create_directories(p.parent_path());
  }
  std::ofstream f(path);
  if (!f) {
    throw std::runtime_error("failed to open output file: " + path);
  }
  f << text;
}

}  // namespace qrs
