#include "qrs_bench/slh_dsa_openssl.h"

#include "qrs_bench/bench_timer.h"

#include <array>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#if defined(QRS_BENCH_HAVE_OPENSSL)
#include <openssl/core_names.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/opensslv.h>
#include <openssl/params.h>
#include <openssl/provider.h>
#endif

namespace qrs {

namespace {

#if defined(QRS_BENCH_HAVE_OPENSSL)
using PkeyPtr = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;
using PkeyCtxPtr = std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>;
using MdCtxPtr = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;

std::string openssl_error() {
  char buf[256];
  const unsigned long err = ERR_get_error();
  if (err == 0) {
    return "OpenSSL call failed without error stack detail";
  }
  ERR_error_string_n(err, buf, sizeof(buf));
  return buf;
}

PkeyPtr generate_key() {
  PkeyCtxPtr ctx(EVP_PKEY_CTX_new_from_name(nullptr, "SLH-DSA-SHA2-128s", nullptr),
                 EVP_PKEY_CTX_free);
  if (!ctx) {
    throw std::runtime_error("EVP_PKEY_CTX_new_from_name failed: " + openssl_error());
  }
  if (EVP_PKEY_keygen_init(ctx.get()) <= 0) {
    throw std::runtime_error("EVP_PKEY_keygen_init failed: " + openssl_error());
  }
  EVP_PKEY* raw = nullptr;
  if (EVP_PKEY_generate(ctx.get(), &raw) <= 0) {
    throw std::runtime_error("EVP_PKEY_generate failed: " + openssl_error());
  }
  return PkeyPtr(raw, EVP_PKEY_free);
}

std::string provider_name(EVP_PKEY* key) {
  const OSSL_PROVIDER* provider = EVP_PKEY_get0_provider(key);
  if (provider == nullptr) {
    return "unavailable";
  }
  const char* name = OSSL_PROVIDER_get0_name(provider);
  return name == nullptr ? "unavailable" : std::string(name);
}

void set_empty_context(EVP_MD_CTX* ctx) {
#if defined(OSSL_SIGNATURE_PARAM_CONTEXT_STRING)
  EVP_PKEY_CTX* pctx = EVP_MD_CTX_get_pkey_ctx(ctx);
  if (pctx == nullptr) {
    throw std::runtime_error("EVP_MD_CTX_get_pkey_ctx failed");
  }
  unsigned char empty_ctx[1] = {0};
  OSSL_PARAM params[] = {
      OSSL_PARAM_construct_octet_string(OSSL_SIGNATURE_PARAM_CONTEXT_STRING, empty_ctx, 0),
      OSSL_PARAM_construct_end(),
  };
  if (EVP_PKEY_CTX_set_params(pctx, params) <= 0) {
    throw std::runtime_error("setting empty SLH-DSA context string failed: " + openssl_error());
  }
#else
  (void)ctx;
#endif
}

std::string slh_dsa_mode() {
#if defined(OSSL_SIGNATURE_PARAM_CONTEXT_STRING)
  return "EVP direct message signing/verification, no prehash digest, context-string explicitly set to empty";
#else
  return "EVP direct message signing/verification, no prehash digest, OpenSSL headers do not expose explicit context-string parameter";
#endif
}

std::vector<unsigned char> raw_public_key(EVP_PKEY* key) {
  size_t len = 0;
  if (EVP_PKEY_get_raw_public_key(key, nullptr, &len) <= 0) {
    throw std::runtime_error("EVP_PKEY_get_raw_public_key size query failed: " + openssl_error());
  }
  std::vector<unsigned char> out(len);
  if (EVP_PKEY_get_raw_public_key(key, out.data(), &len) <= 0) {
    throw std::runtime_error("EVP_PKEY_get_raw_public_key failed: " + openssl_error());
  }
  out.resize(len);
  return out;
}

PkeyPtr import_public_key(const std::vector<unsigned char>& raw_pub) {
  EVP_PKEY* raw = EVP_PKEY_new_raw_public_key_ex(
      nullptr, "SLH-DSA-SHA2-128s", nullptr, raw_pub.data(), raw_pub.size());
  if (raw == nullptr) {
    throw std::runtime_error("EVP_PKEY_new_raw_public_key_ex failed: " + openssl_error());
  }
  return PkeyPtr(raw, EVP_PKEY_free);
}

std::vector<unsigned char> sign_message(EVP_PKEY* key, const std::array<unsigned char, 32>& msg) {
  MdCtxPtr ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
  if (!ctx) {
    throw std::runtime_error("EVP_MD_CTX_new failed");
  }
  if (EVP_DigestSignInit_ex(ctx.get(), nullptr, nullptr, nullptr, nullptr, key, nullptr) <= 0) {
    throw std::runtime_error("EVP_DigestSignInit_ex failed: " + openssl_error());
  }
  set_empty_context(ctx.get());
  size_t sig_len = 0;
  if (EVP_DigestSign(ctx.get(), nullptr, &sig_len, msg.data(), msg.size()) <= 0) {
    throw std::runtime_error("EVP_DigestSign size query failed: " + openssl_error());
  }
  std::vector<unsigned char> sig(sig_len);
  if (EVP_DigestSign(ctx.get(), sig.data(), &sig_len, msg.data(), msg.size()) <= 0) {
    throw std::runtime_error("EVP_DigestSign failed: " + openssl_error());
  }
  sig.resize(sig_len);
  return sig;
}

bool verify_message(EVP_PKEY* key,
                    const std::array<unsigned char, 32>& msg,
                    const std::vector<unsigned char>& sig) {
  MdCtxPtr ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
  if (!ctx) {
    throw std::runtime_error("EVP_MD_CTX_new failed");
  }
  if (EVP_DigestVerifyInit_ex(ctx.get(), nullptr, nullptr, nullptr, nullptr, key, nullptr) <= 0) {
    throw std::runtime_error("EVP_DigestVerifyInit_ex failed: " + openssl_error());
  }
  set_empty_context(ctx.get());
  const int rc = EVP_DigestVerify(ctx.get(), sig.data(), sig.size(), msg.data(), msg.size());
  if (rc < 0) {
    throw std::runtime_error("EVP_DigestVerify error: " + openssl_error());
  }
  return rc == 1;
}
#endif

}  // namespace

SlhDsaResult run_slh_dsa_benchmarks(bool quick) {
  SlhDsaResult r;

#if !defined(QRS_BENCH_HAVE_OPENSSL)
  r.status = "unavailable";
  r.reason = "built without OpenSSL";
  r.valid_verify.status = "unavailable";
  r.invalid_fixed_length_verify.status = "unavailable";
  return r;
#else
  r.openssl_version = OpenSSL_version(OPENSSL_VERSION);
  try {
    auto key = generate_key();
    r.provider = provider_name(key.get());
    const std::vector<unsigned char> raw_pub = raw_public_key(key.get());
    r.public_key_bytes = raw_pub.size();
    if (raw_pub.size() != 32) {
      throw std::runtime_error("SLH-DSA-SHA2-128s public key length was " +
                               std::to_string(raw_pub.size()) + ", expected 32");
    }
    auto public_key = import_public_key(raw_pub);
    std::array<unsigned char, 32> msg {};
    for (std::size_t i = 0; i < msg.size(); ++i) {
      msg[i] = static_cast<unsigned char>(0xa5U ^ i);
    }
    std::vector<unsigned char> sig = sign_message(key.get(), msg);
    if (sig.size() != 7856) {
      throw std::runtime_error("SLH-DSA-SHA2-128s signature length was " +
                               std::to_string(sig.size()) + ", expected 7856");
    }
    r.signature_bytes = sig.size();
    r.mode = slh_dsa_mode();
    if (!verify_message(public_key.get(), msg, sig)) {
      throw std::runtime_error("valid SLH-DSA signature failed before timing");
    }
    std::vector<unsigned char> bad_sig = sig;
    bad_sig[0] ^= 0x01;
    if (verify_message(public_key.get(), msg, bad_sig)) {
      throw std::runtime_error("invalid fixed-length SLH-DSA signature verified before timing");
    }

    const std::size_t batches = quick ? 7 : 31;
    const std::size_t iters = quick ? 8 : 100;
    const std::size_t warmup = quick ? 4 : 32;

    r.valid_verify = summarize(time_batches(
        "slh_dsa_sha2_128s_valid_verify",
        [&]() { return verify_message(public_key.get(), msg, sig); }, batches, iters, warmup));
    r.invalid_fixed_length_verify = summarize(time_batches(
        "slh_dsa_sha2_128s_invalid_fixed_length_verify",
        [&]() { return verify_message(public_key.get(), msg, bad_sig); }, batches, iters, warmup));

    r.status = "available";
    r.reason = "";
    return r;
  } catch (const std::exception& e) {
    r.status = "unavailable";
    r.reason = e.what();
    r.valid_verify.status = "unavailable";
    r.invalid_fixed_length_verify.status = "unavailable";
    return r;
  }
#endif
}

}  // namespace qrs
