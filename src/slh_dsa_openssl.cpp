#include "qrs_bench/slh_dsa_openssl.h"

#include "qrs_bench/bench_timer.h"

#include <array>
#include <algorithm>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
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
  throw std::runtime_error(
      "OpenSSL headers do not expose OSSL_SIGNATURE_PARAM_CONTEXT_STRING; "
      "cannot assert empty SLH-DSA context string");
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

struct InvalidSignatureCase {
  std::string name;
  PkeyPtr public_key;
  std::array<unsigned char, 32> msg;
  std::vector<unsigned char> sig;

  InvalidSignatureCase(std::string name_in,
                       PkeyPtr public_key_in,
                       std::array<unsigned char, 32> msg_in,
                       std::vector<unsigned char> sig_in)
      : name(std::move(name_in)),
        public_key(std::move(public_key_in)),
        msg(msg_in),
        sig(std::move(sig_in)) {}

  InvalidSignatureCase(const InvalidSignatureCase&) = delete;
  InvalidSignatureCase& operator=(const InvalidSignatureCase&) = delete;
  InvalidSignatureCase(InvalidSignatureCase&&) noexcept = default;
  InvalidSignatureCase& operator=(InvalidSignatureCase&&) noexcept = default;
};

std::vector<InvalidSignatureCase> make_invalid_fixed_length_cases(
    const std::vector<unsigned char>& raw_pub,
    const std::array<unsigned char, 32>& msg,
    const std::vector<unsigned char>& sig) {
  std::vector<InvalidSignatureCase> cases;

  auto byte0 = sig;
  byte0[0] ^= 0x01;
  cases.emplace_back("byte0_bitflip", import_public_key(raw_pub), msg, std::move(byte0));

  auto middle = sig;
  middle[middle.size() / 2] ^= 0x02;
  cases.emplace_back("middle_bitflip", import_public_key(raw_pub), msg, std::move(middle));

  auto last = sig;
  last.back() ^= 0x04;
  cases.emplace_back("last_bitflip", import_public_key(raw_pub), msg, std::move(last));

  auto spread = sig;
  for (std::size_t i = 0; i < 32; ++i) {
    const std::size_t pos = (17 + i * 251) % spread.size();
    spread[pos] ^= static_cast<unsigned char>(1U << (i % 8));
  }
  cases.emplace_back("spread_32_bitflips", import_public_key(raw_pub), msg, std::move(spread));

  cases.emplace_back("all_zero", import_public_key(raw_pub), msg,
                     std::vector<unsigned char>(sig.size(), 0x00));
  cases.emplace_back("all_ff", import_public_key(raw_pub), msg,
                     std::vector<unsigned char>(sig.size(), 0xff));

  std::vector<unsigned char> deterministic_garbage(sig.size());
  for (std::size_t i = 0; i < deterministic_garbage.size(); ++i) {
    deterministic_garbage[i] = static_cast<unsigned char>((7 + i * 131) & 0xffU);
  }
  cases.emplace_back("deterministic_garbage", import_public_key(raw_pub), msg,
                     std::move(deterministic_garbage));

  auto wrong_msg = msg;
  wrong_msg[0] ^= 0x80;
  cases.emplace_back("wrong_message", import_public_key(raw_pub), wrong_msg, sig);

  auto wrong_key = generate_key();
  const std::vector<unsigned char> wrong_raw_pub = raw_public_key(wrong_key.get());
  if (wrong_raw_pub.size() != raw_pub.size()) {
    throw std::runtime_error("wrong-public-key SLH-DSA case changed public key length");
  }
  cases.emplace_back("wrong_public_key", import_public_key(wrong_raw_pub), msg, sig);

  return cases;
}

void select_invalid_summary_stats(SlhDsaResult& r) {
  std::vector<NamedStats> available;
  for (const auto& c : r.invalid_fixed_length_cases) {
    if (c.stats.available) {
      available.push_back(c);
    }
  }
  if (available.empty()) {
    return;
  }
  std::sort(available.begin(), available.end(), [](const NamedStats& a, const NamedStats& b) {
    return a.stats.p99_ns < b.stats.p99_ns;
  });
  r.invalid_fixed_length_min_observed = available.front().stats;
  r.invalid_fixed_length_best_observed = available.front().stats;
  r.invalid_fixed_length_median_observed = available[available.size() / 2].stats;
  r.invalid_fixed_length_p99_observed = available.back().stats;
  r.invalid_fixed_length_worst_observed = available.back().stats;
  r.invalid_fixed_length_case_name_worst = available.back().name;
  r.invalid_fixed_length_verify = r.invalid_fixed_length_worst_observed;
}
#endif

}  // namespace

SlhDsaResult run_slh_dsa_benchmarks(BenchmarkMode mode) {
  SlhDsaResult r;
  SlhDsaBackendInfo openssl_backend;
  openssl_backend.name = "openssl";
  SlhDsaBackendInfo second_backend;
  second_backend.name = "second_reviewed_backend";
  second_backend.status = "unavailable";
  second_backend.reason = "second reviewed SLH-DSA backend not wired";
  second_backend.provider = "unavailable";
  second_backend.version = "unavailable";

#if !defined(QRS_BENCH_HAVE_OPENSSL)
  r.status = "unavailable";
  r.reason = "built without OpenSSL";
  openssl_backend.status = "unavailable";
  openssl_backend.reason = r.reason;
  r.valid_verify.status = "unavailable";
  r.invalid_fixed_length_verify.status = "unavailable";
  r.invalid_fixed_length_min_observed.status = "unavailable";
  r.invalid_fixed_length_best_observed.status = "unavailable";
  r.invalid_fixed_length_median_observed.status = "unavailable";
  r.invalid_fixed_length_p99_observed.status = "unavailable";
  r.invalid_fixed_length_worst_observed.status = "unavailable";
  r.backends = {openssl_backend, second_backend};
  return r;
#else
  r.openssl_version = OpenSSL_version(OPENSSL_VERSION);
  openssl_backend.version = r.openssl_version;
  try {
    auto key = generate_key();
    r.provider = provider_name(key.get());
    openssl_backend.provider = r.provider;
    const std::vector<unsigned char> raw_pub = raw_public_key(key.get());
    r.public_key_bytes = raw_pub.size();
    openssl_backend.public_key_bytes = raw_pub.size();
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
    openssl_backend.signature_bytes = sig.size();
    r.mode = slh_dsa_mode();
    if (!verify_message(public_key.get(), msg, sig)) {
      throw std::runtime_error("valid SLH-DSA signature failed before timing");
    }
    std::vector<InvalidSignatureCase> invalid_cases =
        make_invalid_fixed_length_cases(raw_pub, msg, sig);
    for (const auto& c : invalid_cases) {
      if (c.sig.size() != sig.size()) {
        throw std::runtime_error("invalid SLH-DSA case " + c.name +
                                 " changed signature length");
      }
      if (verify_message(c.public_key.get(), c.msg, c.sig)) {
        throw std::runtime_error("invalid fixed-length SLH-DSA signature case " + c.name +
                                 " verified before timing");
      }
    }

    const std::size_t batches =
        mode == BenchmarkMode::Quick ? 7 : (mode == BenchmarkMode::Standard ? 15 : 31);
    const std::size_t iters =
        mode == BenchmarkMode::Quick ? 8 : (mode == BenchmarkMode::Standard ? 32 : 100);
    const std::size_t warmup =
        mode == BenchmarkMode::Quick ? 4 : (mode == BenchmarkMode::Standard ? 8 : 32);

    r.valid_verify = summarize(time_batches(
        "slh_dsa_sha2_128s_valid_verify",
        [&]() { return verify_message(public_key.get(), msg, sig); }, batches, iters, warmup));
    for (const auto& c : invalid_cases) {
      NamedStats timed;
      timed.name = c.name;
      timed.stats = summarize(time_batches(
          "slh_dsa_sha2_128s_invalid_fixed_length_" + c.name + "_verify",
          [&]() { return verify_message(c.public_key.get(), c.msg, c.sig); }, batches, iters,
          warmup));
      r.invalid_fixed_length_cases.push_back(std::move(timed));
    }
    select_invalid_summary_stats(r);

    r.status = "available";
    r.reason = "";
    openssl_backend.status = "available";
    openssl_backend.reason = "";
    r.backends = {openssl_backend, second_backend};
    return r;
  } catch (const std::exception& e) {
    r.status = "unavailable";
    r.reason = e.what();
    openssl_backend.status = "unavailable";
    openssl_backend.reason = r.reason;
    openssl_backend.provider = r.provider;
    openssl_backend.public_key_bytes = r.public_key_bytes;
    openssl_backend.signature_bytes = r.signature_bytes;
    r.valid_verify.status = "unavailable";
    r.invalid_fixed_length_verify.status = "unavailable";
    r.invalid_fixed_length_min_observed.status = "unavailable";
    r.invalid_fixed_length_best_observed.status = "unavailable";
    r.invalid_fixed_length_median_observed.status = "unavailable";
    r.invalid_fixed_length_p99_observed.status = "unavailable";
    r.invalid_fixed_length_worst_observed.status = "unavailable";
    r.backends = {openssl_backend, second_backend};
    return r;
  }
#endif
}

}  // namespace qrs
