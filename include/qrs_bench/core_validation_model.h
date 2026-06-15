#pragma once

#include "qrs_bench/stats.h"

#include <string>

namespace qrs {

struct QrsValidationPathResult {
  std::string status = "unavailable";
  std::string reason;
  std::string scope =
      "Modeled validation path only; not Bitcoin Core consensus integration.";
  std::string qrs_msg_scope =
      "Modeled qrs_msg construction over deterministic fixture fields; final BIP-360 sighash integration remains a blocker.";
  Stats structural_checks;
  Stats qrs_msg_construction;
  Stats verifier_valid;
  Stats total_valid;
  Stats invalid_structural;
  Stats invalid_fixed_length;
};

QrsValidationPathResult run_qrs_validation_path_model(bool quick);

}  // namespace qrs
