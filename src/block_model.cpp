#include "qrs_bench/block_model.h"

#include <cmath>

namespace qrs {

namespace {

BlockEstimate estimate(long input_weight, long block_weight, const Stats* stats) {
  BlockEstimate e;
  e.input_weight_wu = input_weight;
  e.max_inputs = block_weight / input_weight;
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
