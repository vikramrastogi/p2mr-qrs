#include "qrs_bench/slh_dsa_backend.h"

namespace qrs {

static_assert(sizeof(SlhDsaKeypair) >= sizeof(std::vector<unsigned char>),
              "SLH-DSA null backend reserves the keypair transport type");

}  // namespace qrs
