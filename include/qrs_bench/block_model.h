#pragma once

#include "qrs_bench/stats.h"

#include <map>
#include <string>

namespace qrs {

struct BlockEstimate {
  bool available = false;
  std::string status = "unavailable";
  long input_weight_wu = 0;
  long max_inputs = 0;
  std::map<std::string, double> ms;
};

struct BlockModel {
  long block_weight_wu = 4000000;
  long p2tr_keypath_input_wu = 230;
  long p2mr_qrs_depth0_input_wu = 8059;
  BlockEstimate qrs_saturated_block_valid;
  BlockEstimate qrs_saturated_block_invalid_fixed_length;
  BlockEstimate schnorr_individual_saturated_block;
  BlockEstimate schnorr_experimental_batch_saturated_block;
  BlockEstimate schnorr_batch_saturated_block;
};

BlockModel build_block_model(const Stats* qrs_valid,
                             const Stats* qrs_invalid,
                             const Stats* schnorr_individual,
                             const Stats* schnorr_experimental_batch_per_sig,
                             const Stats* schnorr_batch_per_sig);

}  // namespace qrs
