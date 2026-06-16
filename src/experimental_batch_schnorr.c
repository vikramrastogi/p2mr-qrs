#include "qrs_bench/experimental_batch_schnorr.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "assumptions.h"
#include "int128_impl.h"
#include "field_impl.h"
#include "scalar_impl.h"
#include "group_impl.h"
#include "scratch_impl.h"
#include "ecmult_impl.h"
#include "hash_impl.h"

struct qrs_exp_bip340_batch_context {
    size_t capacity;
    secp256k1_scalar g_scalar;
    secp256k1_scalar* scalars;
    secp256k1_ge* points;
    secp256k1_scratch* scratch;
    secp256k1_hash_ctx hash_ctx;
};

typedef struct {
    const secp256k1_scalar* scalars;
    const secp256k1_ge* points;
} qrs_exp_mult_data;

static void qrs_u64_write_le(unsigned char out[8], uint64_t v) {
    size_t i;
    for (i = 0; i < 8; ++i) {
        out[i] = (unsigned char)((v >> (8 * i)) & 0xffU);
    }
}

static void qrs_bip340_challenge_hash_tagged(const secp256k1_hash_ctx* hash_ctx,
                                             unsigned char out32[32],
                                             const unsigned char* r32,
                                             const unsigned char* msg32,
                                             const unsigned char* pubkey32) {
    static const unsigned char tag[] = "BIP0340/challenge";
    secp256k1_sha256 sha;
    secp256k1_sha256_initialize_tagged(hash_ctx, &sha, tag, sizeof(tag) - 1);
    secp256k1_sha256_write(hash_ctx, &sha, r32, 32);
    secp256k1_sha256_write(hash_ctx, &sha, pubkey32, 32);
    secp256k1_sha256_write(hash_ctx, &sha, msg32, 32);
    secp256k1_sha256_finalize(hash_ctx, &sha, out32);
}

static void qrs_bip340_challenge_hash_independent(const secp256k1_hash_ctx* hash_ctx,
                                                  unsigned char out32[32],
                                                  const unsigned char* r32,
                                                  const unsigned char* msg32,
                                                  const unsigned char* pubkey32) {
    static const unsigned char tag[] = "BIP0340/challenge";
    unsigned char tag_hash[32];
    secp256k1_sha256 sha;
    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(hash_ctx, &sha, tag, sizeof(tag) - 1);
    secp256k1_sha256_finalize(hash_ctx, &sha, tag_hash);

    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(hash_ctx, &sha, tag_hash, 32);
    secp256k1_sha256_write(hash_ctx, &sha, tag_hash, 32);
    secp256k1_sha256_write(hash_ctx, &sha, r32, 32);
    secp256k1_sha256_write(hash_ctx, &sha, pubkey32, 32);
    secp256k1_sha256_write(hash_ctx, &sha, msg32, 32);
    secp256k1_sha256_finalize(hash_ctx, &sha, out32);
}

static void qrs_bip340_challenge(const secp256k1_hash_ctx* hash_ctx,
                                 secp256k1_scalar* e,
                                 const unsigned char* r32,
                                 const unsigned char* msg32,
                                 const unsigned char* pubkey32) {
    unsigned char buf[32];
    qrs_bip340_challenge_hash_tagged(hash_ctx, buf, r32, msg32, pubkey32);
    secp256k1_scalar_set_b32(e, buf, NULL);
}

int qrs_exp_bip340_challenge_self_test(void) {
    secp256k1_hash_ctx hash_ctx;
    unsigned char r32[32];
    unsigned char msg32[32];
    unsigned char pubkey32[32];
    unsigned char tagged[32];
    unsigned char independent[32];
    unsigned char scalar_tagged[32];
    unsigned char scalar_independent[32];
    secp256k1_scalar e_tagged;
    secp256k1_scalar e_independent;
    size_t i;

    secp256k1_hash_ctx_init(&hash_ctx);
    for (i = 0; i < 32; ++i) {
        r32[i] = (unsigned char)(0x11U + 3U * i);
        msg32[i] = (unsigned char)(0x42U ^ (7U * i));
        pubkey32[i] = (unsigned char)(0x80U + 5U * i);
    }

    qrs_bip340_challenge_hash_tagged(&hash_ctx, tagged, r32, msg32, pubkey32);
    qrs_bip340_challenge_hash_independent(&hash_ctx, independent, r32, msg32, pubkey32);
    if (memcmp(tagged, independent, 32) != 0) {
        return 0;
    }

    qrs_bip340_challenge(&hash_ctx, &e_tagged, r32, msg32, pubkey32);
    secp256k1_scalar_set_b32(&e_independent, independent, NULL);
    secp256k1_scalar_get_b32(scalar_tagged, &e_tagged);
    secp256k1_scalar_get_b32(scalar_independent, &e_independent);
    return memcmp(scalar_tagged, scalar_independent, 32) == 0;
}

int qrs_exp_bip340_challenge_self_test_with_input(const unsigned char* sig64,
                                                  const unsigned char* msg32,
                                                  const unsigned char* pubkey32) {
    secp256k1_hash_ctx hash_ctx;
    unsigned char tagged[32];
    unsigned char independent[32];
    unsigned char scalar_tagged[32];
    unsigned char scalar_independent[32];
    secp256k1_scalar e_tagged;
    secp256k1_scalar e_independent;

    if (sig64 == NULL || msg32 == NULL || pubkey32 == NULL) {
        return 0;
    }

    secp256k1_hash_ctx_init(&hash_ctx);
    qrs_bip340_challenge_hash_tagged(&hash_ctx, tagged, sig64, msg32, pubkey32);
    qrs_bip340_challenge_hash_independent(&hash_ctx, independent, sig64, msg32, pubkey32);
    if (memcmp(tagged, independent, 32) != 0) {
        return 0;
    }

    qrs_bip340_challenge(&hash_ctx, &e_tagged, sig64, msg32, pubkey32);
    secp256k1_scalar_set_b32(&e_independent, independent, NULL);
    secp256k1_scalar_get_b32(scalar_tagged, &e_tagged);
    secp256k1_scalar_get_b32(scalar_independent, &e_independent);
    return memcmp(scalar_tagged, scalar_independent, 32) == 0;
}

static void qrs_batch_seed(const secp256k1_hash_ctx* hash_ctx,
                           unsigned char out32[32],
                           size_t n,
                           const unsigned char* sig64_flat,
                           const unsigned char* msg32_flat,
                           const unsigned char* pubkey32_flat) {
    static const unsigned char tag[] =
        "QRS bench experimental BIP340 batch coefficients/v0";
    unsigned char n_le[8];
    size_t i;
    secp256k1_sha256 sha;
    secp256k1_sha256_initialize_tagged(hash_ctx, &sha, tag, sizeof(tag) - 1);
    qrs_u64_write_le(n_le, (uint64_t)n);
    secp256k1_sha256_write(hash_ctx, &sha, n_le, sizeof(n_le));
    for (i = 0; i < n; ++i) {
        secp256k1_sha256_write(hash_ctx, &sha, sig64_flat + 64 * i, 64);
        secp256k1_sha256_write(hash_ctx, &sha, msg32_flat + 32 * i, 32);
        secp256k1_sha256_write(hash_ctx, &sha, pubkey32_flat + 32 * i, 32);
    }
    secp256k1_sha256_finalize(hash_ctx, &sha, out32);
}

static void qrs_batch_coefficient(const secp256k1_hash_ctx* hash_ctx,
                                  secp256k1_scalar* a,
                                  const unsigned char seed32[32],
                                  size_t idx) {
    static const unsigned char tag[] =
        "QRS bench experimental BIP340 batch coefficient/v0";
    unsigned char idx_le[8];
    unsigned char buf[32];
    secp256k1_sha256 sha;
    secp256k1_sha256_initialize_tagged(hash_ctx, &sha, tag, sizeof(tag) - 1);
    secp256k1_sha256_write(hash_ctx, &sha, seed32, 32);
    qrs_u64_write_le(idx_le, (uint64_t)idx);
    secp256k1_sha256_write(hash_ctx, &sha, idx_le, sizeof(idx_le));
    secp256k1_sha256_finalize(hash_ctx, &sha, buf);
    secp256k1_scalar_set_b32(a, buf, NULL);
    if (secp256k1_scalar_is_zero(a)) {
        *a = secp256k1_scalar_one;
    }
}

static int qrs_exp_mult_cb(secp256k1_scalar* sc, secp256k1_ge* pt, size_t idx, void* data) {
    const qrs_exp_mult_data* d = (const qrs_exp_mult_data*)data;
    *sc = d->scalars[idx];
    *pt = d->points[idx];
    return 1;
}

qrs_exp_bip340_batch_context* qrs_exp_bip340_batch_context_create(size_t capacity) {
    qrs_exp_bip340_batch_context* ctx;
    size_t scratch_size;
    if (capacity == 0 || capacity > ((size_t)-1) / 2 ||
        capacity > (((size_t)-1) - 1024 * 1024) / 768) {
        return NULL;
    }
    ctx = (qrs_exp_bip340_batch_context*)calloc(1, sizeof(*ctx));
    if (ctx == NULL) {
        return NULL;
    }
    ctx->capacity = capacity;
    ctx->scalars = (secp256k1_scalar*)calloc(2 * capacity, sizeof(*ctx->scalars));
    ctx->points = (secp256k1_ge*)calloc(2 * capacity, sizeof(*ctx->points));
    scratch_size = 1024 * 1024 + capacity * 768;
    ctx->scratch = secp256k1_scratch_create(&default_error_callback, scratch_size);
    if (ctx->scalars == NULL || ctx->points == NULL || ctx->scratch == NULL) {
        qrs_exp_bip340_batch_context_destroy(ctx);
        return NULL;
    }
    secp256k1_hash_ctx_init(&ctx->hash_ctx);
    return ctx;
}

void qrs_exp_bip340_batch_context_destroy(qrs_exp_bip340_batch_context* ctx) {
    if (ctx == NULL) {
        return;
    }
    if (ctx->scratch != NULL) {
        secp256k1_scratch_destroy(&default_error_callback, ctx->scratch);
    }
    free(ctx->points);
    free(ctx->scalars);
    free(ctx);
}

int qrs_exp_bip340_batch_verify(qrs_exp_bip340_batch_context* ctx,
                                size_t n,
                                const unsigned char* sig64_flat,
                                const unsigned char* msg32_flat,
                                const unsigned char* pubkey32_flat) {
    unsigned char seed32[32];
    secp256k1_gej result;
    qrs_exp_mult_data mult_data;
    size_t i;
    if (ctx == NULL || n == 0 || n > ctx->capacity ||
        sig64_flat == NULL || msg32_flat == NULL || pubkey32_flat == NULL) {
        return 0;
    }

    qrs_batch_seed(&ctx->hash_ctx, seed32, n, sig64_flat, msg32_flat, pubkey32_flat);
    secp256k1_scalar_set_int(&ctx->g_scalar, 0);

    for (i = 0; i < n; ++i) {
        const unsigned char* sig = sig64_flat + 64 * i;
        const unsigned char* msg = msg32_flat + 32 * i;
        const unsigned char* pk32 = pubkey32_flat + 32 * i;
        secp256k1_fe rx;
        secp256k1_fe px;
        secp256k1_ge r_point;
        secp256k1_ge p_point;
        secp256k1_scalar s;
        secp256k1_scalar e;
        secp256k1_scalar a;
        secp256k1_scalar term;
        int overflow = 0;

        if (!secp256k1_fe_set_b32_limit(&rx, sig)) {
            return 0;
        }
        if (!secp256k1_ge_set_xo_var(&r_point, &rx, 0)) {
            return 0;
        }
        secp256k1_scalar_set_b32(&s, sig + 32, &overflow);
        if (overflow) {
            return 0;
        }
        if (!secp256k1_fe_set_b32_limit(&px, pk32)) {
            return 0;
        }
        if (!secp256k1_ge_set_xo_var(&p_point, &px, 0)) {
            return 0;
        }

        if (i == 0) {
            a = secp256k1_scalar_one;
        } else {
            qrs_batch_coefficient(&ctx->hash_ctx, &a, seed32, i);
        }
        qrs_bip340_challenge(&ctx->hash_ctx, &e, sig, msg, pk32);

        secp256k1_scalar_mul(&term, &a, &s);
        secp256k1_scalar_add(&ctx->g_scalar, &ctx->g_scalar, &term);

        ctx->points[2 * i] = r_point;
        secp256k1_scalar_negate(&ctx->scalars[2 * i], &a);

        ctx->points[2 * i + 1] = p_point;
        secp256k1_scalar_mul(&term, &a, &e);
        secp256k1_scalar_negate(&ctx->scalars[2 * i + 1], &term);
    }

    mult_data.scalars = ctx->scalars;
    mult_data.points = ctx->points;
    if (!secp256k1_ecmult_multi_var(&default_error_callback,
                                    ctx->scratch,
                                    &result,
                                    &ctx->g_scalar,
                                    qrs_exp_mult_cb,
                                    &mult_data,
                                    2 * n)) {
        return 0;
    }
    return secp256k1_gej_is_infinity(&result);
}
