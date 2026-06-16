#pragma once

#include <string>

namespace qrs {

struct QrsDigestVectorResult {
  std::string leaf_hash;
  std::string merkle_root;
  std::string qrs_msg;
  bool reaches_slh_dsa_verifier = false;
};

bool is_qrs_digest_cli_command(const std::string& command);
int run_qrs_digest_cli(int argc, char** argv);
QrsDigestVectorResult compute_qrs_digest_vector_file(const std::string& path);

}  // namespace qrs
