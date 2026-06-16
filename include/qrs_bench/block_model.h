#pragma once

#include "qrs_bench/stats.h"

#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace qrs {

struct BlockEstimate {
  bool available = false;
  std::string status = "unavailable";
  std::size_t qrs_merkle_depth = 0;
  long merkle_path_hashes_per_input = 0;
  long merkle_path_hashes_per_block = 0;
  long input_weight_wu = 0;
  long max_inputs = 0;
  std::map<std::string, double> ms;
};

struct BlockModel {
  long block_weight_wu = 4000000;
  long p2tr_keypath_input_wu = 230;
  long p2mr_qrs_depth0_input_wu = 8059;
  long p2mr_qrs_control_path_wu_per_depth = 32;
  BlockEstimate qrs_saturated_block_valid;
  BlockEstimate qrs_saturated_block_invalid_fixed_length;
  std::vector<BlockEstimate> qrs_saturated_block_valid_by_depth;
  std::vector<BlockEstimate> qrs_saturated_block_invalid_fixed_length_by_depth;
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
