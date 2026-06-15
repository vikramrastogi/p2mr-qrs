#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

namespace qrs {

extern volatile unsigned long long verification_sink;

struct TimingSeries {
  std::string name;
  std::vector<double> per_op_ns;
  std::size_t batches = 0;
  std::size_t iterations_per_batch = 0;
};

TimingSeries time_batches(const std::string& name,
                          const std::function<bool()>& fn,
                          std::size_t batches,
                          std::size_t iterations_per_batch,
                          std::size_t warmup_iterations);

}  // namespace qrs

