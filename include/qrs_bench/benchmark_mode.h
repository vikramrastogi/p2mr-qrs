#pragma once

#include <string>

namespace qrs {

enum class BenchmarkMode {
  Quick,
  Standard,
  Full,
};

inline std::string benchmark_mode_name(BenchmarkMode mode) {
  switch (mode) {
    case BenchmarkMode::Quick:
      return "quick";
    case BenchmarkMode::Standard:
      return "standard";
    case BenchmarkMode::Full:
      return "full";
  }
  return "unknown";
}

}  // namespace qrs
