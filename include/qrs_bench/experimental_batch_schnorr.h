#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct qrs_exp_bip340_batch_context qrs_exp_bip340_batch_context;

qrs_exp_bip340_batch_context* qrs_exp_bip340_batch_context_create(size_t capacity);
void qrs_exp_bip340_batch_context_destroy(qrs_exp_bip340_batch_context* ctx);

int qrs_exp_bip340_batch_verify(qrs_exp_bip340_batch_context* ctx,
                                size_t n,
                                const unsigned char* sig64_flat,
                                const unsigned char* msg32_flat,
                                const unsigned char* pubkey32_flat);

#ifdef __cplusplus
}
#endif
