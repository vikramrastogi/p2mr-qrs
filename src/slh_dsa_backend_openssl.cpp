#include "qrs_bench/slh_dsa_backend.h"

namespace qrs {

static_assert(sizeof(SlhDsaBackend*) == sizeof(void*),
              "SLH-DSA backend interface must remain an abstract dispatch point");

}  // namespace qrs
