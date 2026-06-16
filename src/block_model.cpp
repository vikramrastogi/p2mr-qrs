#include "qrs_bench/block_model.h"

#include <cmath>
#include <cstddef>

namespace qrs {

namespace {

constexpr std::size_t kQrsDepths[] = {0, 1, 3, 128};

BlockEstimate estimate(long input_weight,
                       long block_weight,
                       const Stats* stats,
                       std::size_t qrs_merkle_depth = 0) {
  BlockEstimate e;
  e.qrs_merkle_depth = qrs_merkle_depth;
  e.input_weight_wu = input_weight;
  e.max_inputs = block_weight / input_weight;
  e.merkle_path_hashes_per_input = static_cast<long>(qrs_merkle_depth);
  e.merkle_path_hashes_per_block =
      e.merkle_path_hashes_per_input * e.max_inputs;
  if (stats == nullptr || !stats->available) {
    e.available = false;
    e.status = "unavailable";
    return e;
  }
  e.available = true;
  e.status = "available";
  e.ms["median_ms"] = (stats->median_ns * e.max_inputs) / 1'000'000.0;
  e.ms["p95_ms"] = (stats->p95_ns * e.max_inputs) / 1'000'000.0;
  e.ms["p99_ms"] = (stats->p99_ns * e.max_inputs) / 1'000'000.0;
  e.ms["max_ms"] = (stats->max_ns * e.max_inputs) / 1'000'000.0;
  return e;
}

long qrs_input_weight_at_depth(const BlockModel& model, std::size_t depth) {
  return model.p2mr_qrs_depth0_input_wu +
         static_cast<long>(depth) * model.p2mr_qrs_control_path_wu_per_depth;
}

}  // namespace

BlockModel build_block_model(const Stats* qrs_valid,
                             const Stats* qrs_invalid,
                             const Stats* schnorr_individual,
                             const Stats* schnorr_experimental_batch_per_sig,
                             const Stats* schnorr_batch_per_sig) {
  BlockModel model;
  model.qrs_saturated_block_valid =
      estimate(model.p2mr_qrs_depth0_input_wu, model.block_weight_wu, qrs_valid);
  model.qrs_saturated_block_invalid_fixed_length =
      estimate(model.p2mr_qrs_depth0_input_wu, model.block_weight_wu, qrs_invalid);
  for (const std::size_t depth : kQrsDepths) {
    const long input_weight = qrs_input_weight_at_depth(model, depth);
    model.qrs_saturated_block_valid_by_depth.push_back(
        estimate(input_weight, model.block_weight_wu, qrs_valid, depth));
    model.qrs_saturated_block_invalid_fixed_length_by_depth.push_back(
        estimate(input_weight, model.block_weight_wu, qrs_invalid, depth));
  }
  model.schnorr_individual_saturated_block =
      estimate(model.p2tr_keypath_input_wu, model.block_weight_wu, schnorr_individual);
  model.schnorr_experimental_batch_saturated_block =
      estimate(model.p2tr_keypath_input_wu, model.block_weight_wu,
               schnorr_experimental_batch_per_sig);
  model.schnorr_batch_saturated_block =
      estimate(model.p2tr_keypath_input_wu, model.block_weight_wu, schnorr_batch_per_sig);
  return model;
}

}  // namespace qrs
