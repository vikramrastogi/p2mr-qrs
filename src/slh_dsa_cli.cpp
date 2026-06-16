#include "qrs_bench/slh_dsa_cli.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#if defined(QRS_BENCH_HAVE_OPENSSL)
#include <openssl/core_names.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/params.h>
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

unsigned char hex_nibble(char c) {
  if (c >= '0' && c <= '9') return static_cast<unsigned char>(c - '0');
  if (c >= 'a' && c <= 'f') return static_cast<unsigned char>(10 + c - 'a');
  if (c >= 'A' && c <= 'F') return static_cast<unsigned char>(10 + c - 'A');
  throw std::runtime_error("invalid hex character");
}

std::vector<unsigned char> parse_hex(const std::string& hex, const std::string& field) {
  if ((hex.size() % 2) != 0) {
    throw std::runtime_error(field + " must have an even number of hex characters");
  }
  std::vector<unsigned char> out(hex.size() / 2);
  for (std::size_t i = 0; i < out.size(); ++i) {
    out[i] = static_cast<unsigned char>((hex_nibble(hex[2 * i]) << 4) |
                                        hex_nibble(hex[2 * i + 1]));
  }
  return out;
}

std::string to_hex(const std::vector<unsigned char>& bytes) {
  static constexpr char kHex[] = "0123456789abcdef";
  std::string out;
  out.reserve(bytes.size() * 2);
  for (const unsigned char b : bytes) {
    out.push_back(kHex[b >> 4]);
    out.push_back(kHex[b & 0x0f]);
  }
  return out;
}

std::string arg_value(int argc, char** argv, const std::string& name) {
  for (int i = 2; i + 1 < argc; ++i) {
    if (argv[i] == name) {
      return argv[i + 1];
    }
  }
  throw std::runtime_error("missing required argument " + name);
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

std::vector<unsigned char> raw_private_key(EVP_PKEY* key) {
  size_t len = 0;
  if (EVP_PKEY_get_raw_private_key(key, nullptr, &len) <= 0) {
    throw std::runtime_error("EVP_PKEY_get_raw_private_key size query failed: " + openssl_error());
  }
  std::vector<unsigned char> out(len);
  if (EVP_PKEY_get_raw_private_key(key, out.data(), &len) <= 0) {
    throw std::runtime_error("EVP_PKEY_get_raw_private_key failed: " + openssl_error());
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

PkeyPtr import_private_key(const std::vector<unsigned char>& raw_priv) {
  EVP_PKEY* raw = EVP_PKEY_new_raw_private_key_ex(
      nullptr, "SLH-DSA-SHA2-128s", nullptr, raw_priv.data(), raw_priv.size());
  if (raw == nullptr) {
    throw std::runtime_error("EVP_PKEY_new_raw_private_key_ex failed: " + openssl_error());
  }
  return PkeyPtr(raw, EVP_PKEY_free);
}

std::vector<unsigned char> sign_message(EVP_PKEY* key, const std::vector<unsigned char>& msg) {
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
                    const std::vector<unsigned char>& msg,
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

int keygen_command() {
  auto key = generate_key();
  const auto pub = raw_public_key(key.get());
  const auto priv = raw_private_key(key.get());
  if (pub.size() != 32) {
    throw std::runtime_error("SLH-DSA-SHA2-128s public key length was not 32 bytes");
  }
  std::cout << "{\n"
            << "  \"public_key_hex\": \"" << to_hex(pub) << "\",\n"
            << "  \"private_key_hex\": \"" << to_hex(priv) << "\"\n"
            << "}\n";
  return 0;
}

int sign_command(int argc, char** argv) {
  const auto priv = parse_hex(arg_value(argc, argv, "--private-key-hex"), "private key");
  const auto msg = parse_hex(arg_value(argc, argv, "--message-hex"), "message");
  auto key = import_private_key(priv);
  const auto sig = sign_message(key.get(), msg);
  if (sig.size() != 7856) {
    throw std::runtime_error("SLH-DSA-SHA2-128s signature length was not 7856 bytes");
  }
  std::cout << to_hex(sig) << "\n";
  return 0;
}

int verify_command(int argc, char** argv) {
  const auto pub = parse_hex(arg_value(argc, argv, "--public-key-hex"), "public key");
  const auto msg = parse_hex(arg_value(argc, argv, "--message-hex"), "message");
  const auto sig = parse_hex(arg_value(argc, argv, "--signature-hex"), "signature");
  auto key = import_public_key(pub);
  std::cout << (verify_message(key.get(), msg, sig) ? "valid" : "invalid") << "\n";
  return 0;
}
#endif

}  // namespace

bool is_slh_dsa_cli_command(const std::string& command) {
  return command == "--slh-keygen" || command == "--slh-sign" || command == "--slh-verify";
}

int run_slh_dsa_cli(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "missing SLH-DSA command\n";
    return 3;
  }
#if !defined(QRS_BENCH_HAVE_OPENSSL)
  std::cerr << "unavailable: built without OpenSSL\n";
  return 2;
#else
  try {
    const std::string command = argv[1];
    if (command == "--slh-keygen") {
      return keygen_command();
    }
    if (command == "--slh-sign") {
      return sign_command(argc, argv);
    }
    if (command == "--slh-verify") {
      return verify_command(argc, argv);
    }
    std::cerr << "unknown SLH-DSA command: " << command << "\n";
    return 3;
  } catch (const std::exception& e) {
    std::cerr << "unavailable: " << e.what() << "\n";
    return 2;
  }
#endif
}

}  // namespace qrs
