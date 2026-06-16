#pragma once

#include <string>

namespace qrs {

bool is_slh_dsa_cli_command(const std::string& command);
int run_slh_dsa_cli(int argc, char** argv);

}  // namespace qrs
