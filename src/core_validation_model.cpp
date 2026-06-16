#include "qrs_bench/core_validation_model.h"

#include "qrs_bench/bench_timer.h"

#include <array>
#include <memory>
#include <stdexcept>
#include <vector>

#if defined(QRS_BENCH_HAVE_OPENSSL)
#include <openssl/core_names.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/params.h>
#include <openssl/sha.h>
#endif

namespace qrs {

namespace {

constexpr unsigned char kQrsLeafVersion = 0xc2;
constexpr std::size_t kQrsSignatureBytes = 7856;
constexpr std::size_t kQrsPublicKeyBytes = 32;
constexpr std::size_t kQrsMaxDepth = 128;

#if defined(QRS_BENCH_HAVE_OPENSSL)
using PkeyPtr = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;
using PkeyCtxPtr = std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>;
using MdCtxPtr = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;

struct ModeledSpend {
  std::vector<unsigned char> signature;
  std::vector<unsigned char> public_key;
  std::vector<unsigned char> control_block;
  std::vector<unsigned char> annex;
  std::array<unsigned char, 32> expected_root {};
};

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

std::array<unsigned char, 32> sha256_bytes(const std::vector<unsigned char>& bytes) {
  std::array<unsigned char, 32> out {};
  SHA256(bytes.data(), bytes.size(), out.data());
  return out;
}

void append_bytes(std::vector<unsigned char>& out, const std::vector<unsigned char>& in) {
  out.insert(out.end(), in.begin(), in.end());
}

void append_array(std::vector<unsigned char>& out, const std::array<unsigned char, 32>& in) {
  out.insert(out.end(), in.begin(), in.end());
}

std::array<unsigned char, 32> modeled_leaf_hash(const std::vector<unsigned char>& public_key) {
  std::vector<unsigned char> buf;
  const char prefix[] = "QRS modeled TapLeaf v0";
  buf.insert(buf.end(), prefix, prefix + sizeof(prefix) - 1);
  buf.push_back(kQrsLeafVersion);
  buf.push_back(static_cast<unsigned char>(public_key.size()));
  append_bytes(buf, public_key);
  return sha256_bytes(buf);
}

std::array<unsigned char, 32> modeled_root(const std::array<unsigned char, 32>& leaf_hash,
                                           const std::vector<unsigned char>& control_block) {
  std::array<unsigned char, 32> cur = leaf_hash;
  const std::size_t depth = (control_block.size() - 1) / 32;
  for (std::size_t i = 0; i < depth; ++i) {
    std::vector<unsigned char> buf;
    const char prefix[] = "QRS modeled Branch v0";
    buf.insert(buf.end(), prefix, prefix + sizeof(prefix) - 1);
    append_array(buf, cur);
    buf.insert(buf.end(), control_block.begin() + 1 + 32 * i,
               control_block.begin() + 1 + 32 * (i + 1));
    cur = sha256_bytes(buf);
  }
  return cur;
}

bool structural_checks(const ModeledSpend& spend) {
  if (spend.signature.size() != kQrsSignatureBytes) return false;
  if (spend.public_key.size() != kQrsPublicKeyBytes) return false;
  if (spend.control_block.empty()) return false;
  const std::size_t path_bytes = spend.control_block.size() - 1;
  if (path_bytes % 32 != 0 || path_bytes / 32 > kQrsMaxDepth) return false;
  if ((spend.control_block[0] & 0xfeU) != kQrsLeafVersion) return false;
  if ((spend.control_block[0] & 1U) != 1U) return false;
  const auto leaf = modeled_leaf_hash(spend.public_key);
  return modeled_root(leaf, spend.control_block) == spend.expected_root;
}

std::array<unsigned char, 32> construct_qrs_msg(const ModeledSpend& spend) {
  const auto leaf = modeled_leaf_hash(spend.public_key);
  std::vector<unsigned char> buf;
  const char prefix[] = "QRS modeled SigMsg v0";
  buf.insert(buf.end(), prefix, prefix + sizeof(prefix) - 1);
  append_array(buf, leaf);
  append_bytes(buf, spend.control_block);
  append_bytes(buf, spend.annex);
  return sha256_bytes(buf);
}

bool modeled_validation_path(EVP_PKEY* public_key,
                             const ModeledSpend& spend,
                             bool* verifier_called = nullptr) {
  if (verifier_called != nullptr) {
    *verifier_called = false;
  }
  if (!structural_checks(spend)) {
    return false;
  }
  const auto msg = construct_qrs_msg(spend);
  if (verifier_called != nullptr) {
    *verifier_called = true;
  }
  return verify_message(public_key, msg, spend.signature);
}

Stats unavailable_stats() {
  Stats s;
  s.status = "unavailable";
  s.available = false;
  return s;
}
#endif

}  // namespace

QrsValidationPathResult run_qrs_validation_path_model(BenchmarkMode mode) {
  QrsValidationPathResult r;

#if !defined(QRS_BENCH_HAVE_OPENSSL)
  r.status = "unavailable";
  r.reason = "built without OpenSSL";
  return r;
#else
  try {
    auto key = generate_key();
    const std::vector<unsigned char> raw_pub = raw_public_key(key.get());
    if (raw_pub.size() != kQrsPublicKeyBytes) {
      throw std::runtime_error("modeled path public key length assertion failed");
    }
    auto public_key = import_public_key(raw_pub);

    ModeledSpend valid;
    valid.public_key = raw_pub;
    valid.control_block = {static_cast<unsigned char>(kQrsLeafVersion | 0x01U)};
    valid.annex = {0x50, 0x51, 0x52};
    const auto leaf = modeled_leaf_hash(valid.public_key);
    valid.expected_root = modeled_root(leaf, valid.control_block);
    const auto qrs_msg = construct_qrs_msg(valid);
    valid.signature = sign_message(key.get(), qrs_msg);
    if (valid.signature.size() != kQrsSignatureBytes) {
      throw std::runtime_error("modeled path signature length assertion failed");
    }

    ModeledSpend invalid_structural = valid;
    invalid_structural.control_block[0] &= 0xfeU;

    ModeledSpend invalid_fixed = valid;
    invalid_fixed.signature[0] ^= 0x01U;

    if (!structural_checks(valid)) {
      throw std::runtime_error("valid modeled witness failed structural checks");
    }
    if (structural_checks(invalid_structural)) {
      throw std::runtime_error("invalid structural modeled witness passed checks");
    }
    if (!verify_message(public_key.get(), qrs_msg, valid.signature)) {
      throw std::runtime_error("valid modeled verifier call failed");
    }
    if (verify_message(public_key.get(), qrs_msg, invalid_fixed.signature)) {
      throw std::runtime_error("invalid fixed-length modeled signature verified");
    }
    bool verifier_called = false;
    if (!modeled_validation_path(public_key.get(), valid, &verifier_called) || !verifier_called) {
      throw std::runtime_error("valid modeled path failed");
    }
    verifier_called = true;
    if (modeled_validation_path(public_key.get(), invalid_structural, &verifier_called) ||
        verifier_called) {
      throw std::runtime_error("structural reject reached verifier");
    }
    verifier_called = false;
    if (modeled_validation_path(public_key.get(), invalid_fixed, &verifier_called) ||
        !verifier_called) {
      throw std::runtime_error("fixed-length crypto reject did not reach verifier");
    }

    const std::size_t batches =
        mode == BenchmarkMode::Quick ? 7 : (mode == BenchmarkMode::Standard ? 15 : 31);
    const std::size_t verify_iters =
        mode == BenchmarkMode::Quick ? 8 : (mode == BenchmarkMode::Standard ? 32 : 100);
    const std::size_t cheap_iters =
        mode == BenchmarkMode::Quick ? 2000 : (mode == BenchmarkMode::Standard ? 8000 : 20000);
    const std::size_t warmup_verify =
        mode == BenchmarkMode::Quick ? 4 : (mode == BenchmarkMode::Standard ? 8 : 32);
    const std::size_t warmup_cheap =
        mode == BenchmarkMode::Quick ? 200 : (mode == BenchmarkMode::Standard ? 400 : 1000);

    r.structural_checks = summarize(time_batches(
        "qrs_validation_path_structural_checks",
        [&]() { return structural_checks(valid); }, batches, cheap_iters, warmup_cheap));
    r.qrs_msg_construction = summarize(time_batches(
        "qrs_validation_path_qrs_msg_construction",
        [&]() {
          const auto msg = construct_qrs_msg(valid);
          verification_sink += msg[0];
          return true;
        },
        batches, cheap_iters, warmup_cheap));
    r.verifier_valid = summarize(time_batches(
        "qrs_validation_path_verifier_valid",
        [&]() { return verify_message(public_key.get(), qrs_msg, valid.signature); },
        batches, verify_iters, warmup_verify));
    r.total_valid = summarize(time_batches(
        "qrs_validation_path_total_valid",
        [&]() { return modeled_validation_path(public_key.get(), valid); },
        batches, verify_iters, warmup_verify));
    r.invalid_structural = summarize(time_batches(
        "qrs_validation_path_invalid_structural",
        [&]() { return !modeled_validation_path(public_key.get(), invalid_structural); },
        batches, cheap_iters, warmup_cheap));
    r.invalid_fixed_length = summarize(time_batches(
        "qrs_validation_path_invalid_fixed_length",
        [&]() { return !modeled_validation_path(public_key.get(), invalid_fixed); },
        batches, verify_iters, warmup_verify));

    r.status = "available";
    r.reason = "";
    return r;
  } catch (const std::exception& e) {
    r.status = "unavailable";
    r.reason = e.what();
    r.structural_checks = unavailable_stats();
    r.qrs_msg_construction = unavailable_stats();
    r.verifier_valid = unavailable_stats();
    r.total_valid = unavailable_stats();
    r.invalid_structural = unavailable_stats();
    r.invalid_fixed_length = unavailable_stats();
    return r;
  }
#endif
}

}  // namespace qrs
