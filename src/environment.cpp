#include "qrs_bench/environment.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/utsname.h>

#if defined(__APPLE__)
#include <sys/sysctl.h>
#endif

namespace qrs {

namespace {

#if defined(__APPLE__)
std::string sysctl_string(const char* name) {
  size_t size = 0;
  if (sysctlbyname(name, nullptr, &size, nullptr, 0) != 0 || size == 0) {
    return "unavailable";
  }
  std::string value(size, '\0');
  if (sysctlbyname(name, value.data(), &size, nullptr, 0) != 0) {
    return "unavailable";
  }
  while (!value.empty() && value.back() == '\0') {
    value.pop_back();
  }
  return value.empty() ? "unavailable" : value;
}

std::string sysctl_int_string(const char* name) {
  int value = 0;
  size_t size = sizeof(value);
  if (sysctlbyname(name, &value, &size, nullptr, 0) != 0) {
    return "unavailable";
  }
  return std::to_string(value);
}

std::string apple_arm_features() {
  const char* keys[] = {
      "hw.optional.arm.FEAT_AES",
      "hw.optional.arm.FEAT_PMULL",
      "hw.optional.arm.FEAT_SHA1",
      "hw.optional.arm.FEAT_SHA256",
      "hw.optional.arm.FEAT_SHA3",
      "hw.optional.arm.FEAT_SHA512",
      "hw.optional.arm.FEAT_LSE",
      "hw.optional.arm.FEAT_FP16",
  };
  std::string out;
  for (const char* key : keys) {
    const std::string value = sysctl_int_string(key);
    if (value == "unavailable") {
      continue;
    }
    if (!out.empty()) {
      out += ", ";
    }
    out += key;
    out += "=";
    out += value;
  }
  return out.empty() ? "unavailable" : out;
}
#else
std::string command_output(const char* command) {
  std::string out;
  FILE* pipe = popen(command, "r");
  if (pipe == nullptr) {
    return "unavailable";
  }
  char buffer[256];
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    out += buffer;
  }
  pclose(pipe);
  while (!out.empty() && (out.back() == '\n' || out.back() == ' ' || out.back() == '\t')) {
    out.pop_back();
  }
  return out.empty() ? "unavailable" : out;
}
#endif

}  // namespace

Environment collect_environment() {
  Environment env;

  struct utsname uts {};
  if (uname(&uts) == 0) {
    env.fields["os"] = std::string(uts.sysname) + " " + uts.release + " " + uts.machine;
  } else {
    env.fields["os"] = "unavailable";
  }

#if defined(__APPLE__)
  env.fields["cpu_model"] = sysctl_string("machdep.cpu.brand_string");
  env.fields["cpu_features"] = apple_arm_features();
  env.fields["cpu_frequency"] = sysctl_string("hw.cpufrequency");
#else
  env.fields["cpu_model"] = command_output("lscpu | sed -n 's/^Model name:[[:space:]]*//p' | head -1");
  env.fields["cpu_features"] = command_output("lscpu | sed -n 's/^Flags:[[:space:]]*//p' | head -1");
  env.fields["cpu_frequency"] = "unavailable";
#endif

#if defined(__clang__)
  env.fields["compiler"] = std::string("clang ") + __clang_version__;
#elif defined(__GNUC__)
  env.fields["compiler"] = std::string("gcc ") + __VERSION__;
#else
  env.fields["compiler"] = "unknown";
#endif

#if defined(NDEBUG)
  env.fields["build_type"] = "Release/NDEBUG";
#else
  env.fields["build_type"] = "non-release";
#endif

  env.fields["compiler_flags"] = "CMake target: -O3 -Wall -Wextra -Wpedantic";
  env.fields["benchmark_commit"] = "record with git rev-parse HEAD in published runs";
  env.fields["frequency_scaling"] = "not controlled by harness; reviewer must record governor/power mode";
  return env;
}

}  // namespace qrs
