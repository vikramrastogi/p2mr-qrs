#pragma once

#include "qrs_bench/block_model.h"
#include "qrs_bench/core_validation_model.h"
#include "qrs_bench/environment.h"
#include "qrs_bench/slh_dsa_openssl.h"
#include "qrs_bench/schnorr_secp256k1.h"

#include <string>

namespace qrs {

std::string render_markdown(const Environment& env,
                            const SlhDsaResult& slh,
                            const SchnorrResult& schnorr,
                            const QrsValidationPathResult& qrs_path,
                            const BlockModel& block_model,
                            const std::string& mode);

}  // namespace qrs
