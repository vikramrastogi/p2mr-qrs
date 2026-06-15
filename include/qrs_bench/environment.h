#pragma once

#include <map>
#include <string>

namespace qrs {

struct Environment {
  std::map<std::string, std::string> fields;
};

Environment collect_environment();

}  // namespace qrs

