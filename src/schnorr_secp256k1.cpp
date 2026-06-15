#include "qrs_bench/schnorr_secp256k1.h"

#include "qrs_bench/bench_timer.h"
#include "qrs_bench/experimental_batch_schnorr.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <vector>

#if defined(QRS_BENCH_HAVE_SECP256K1)
#include <secp256k1.h>
#include <secp256k1_extrakeys.h>
#include <secp256k1_schnorrsig.h>
#endif

namespace qrs {

namespace {

#if defined(QRS_BENCH_HAVE_SECP256K1)
using ContextPtr = std::unique_ptr<secp256k1_context, decltype(&secp256k1_context_destroy)>;

ContextPtr create_context() {
  secp256k1_context* raw = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
  if (raw == nullptr) {
    throw std::runtime_error("secp256k1_context_create failed");
  }
  return ContextPtr(raw, secp256k1_context_destroy);
}

void fill32(std::array<unsigned char, 32>& out, unsigned char seed) {
  for (std::size_t i = 0; i < out.size(); ++i) {
    out[i] = static_cast<unsigned char>(seed + static_cast<unsigned char>(i * 17U));
  }
}

struct BatchInputs {
  std::size_t size = 0;
  std::vector<unsigned char> sigs;
  std::vector<unsigned char> bad_sigs;
  std::vector<unsigned char> msgs;
  std::vector<unsigned char> pubkeys;
};

using BatchContextPtr =
    std::unique_ptr<qrs_exp_bip340_batch_context,
                    decltype(&qrs_exp_bip340_batch_context_destroy)>;

void fill_indexed32(std::array<unsigned char, 32>& out,
                    std::size_t index,
                    unsigned char domain) {
  for (std::size_t i = 0; i < out.size(); ++i) {
    out[i] = static_cast<unsigned char>(domain ^ static_cast<unsigned char>(i * 29U) ^
                                        static_cast<unsigned char>(index * 13U));
  }
}

std::array<unsigned char, 32> seckey_for_index(std::size_t index) {
  std::array<unsigned char, 32> seckey {};
  const std::uint32_t v = static_cast<std::uint32_t>(index + 1);
  seckey[28] = static_cast<unsigned char>((v >> 24) & 0xffU);
  seckey[29] = static_cast<unsigned char>((v >> 16) & 0xffU);
  seckey[30] = static_cast<unsigned char>((v >> 8) & 0xffU);
  seckey[31] = static_cast<unsigned char>(v & 0xffU);
  return seckey;
}

BatchInputs make_batch_inputs(secp256k1_context* ctx, std::size_t size) {
  BatchInputs inputs;
  inputs.size = size;
  inputs.sigs.resize(size * 64);
  inputs.msgs.resize(size * 32);
  inputs.pubkeys.resize(size * 32);

  for (std::size_t i = 0; i < size; ++i) {
    std::array<unsigned char, 32> seckey = seckey_for_index(i);
    std::array<unsigned char, 32> msg {};
    std::array<unsigned char, 32> aux {};
    fill_indexed32(msg, i, 0x42);
    fill_indexed32(aux, i, 0x80);

    if (secp256k1_ec_seckey_verify(ctx, seckey.data()) != 1) {
      throw std::runtime_error("deterministic batch seckey failed secp256k1 validation");
    }
    std::memcpy(inputs.msgs.data() + 32 * i, msg.data(), msg.size());

    secp256k1_keypair keypair;
    if (secp256k1_keypair_create(ctx, &keypair, seckey.data()) != 1) {
      throw std::runtime_error("batch secp256k1_keypair_create failed");
    }
    secp256k1_xonly_pubkey pubkey;
    int pk_parity = 0;
    if (secp256k1_keypair_xonly_pub(ctx, &pubkey, &pk_parity, &keypair) != 1) {
      throw std::runtime_error("batch secp256k1_keypair_xonly_pub failed");
    }
    if (secp256k1_xonly_pubkey_serialize(ctx, inputs.pubkeys.data() + 32 * i, &pubkey) != 1) {
      throw std::runtime_error("batch secp256k1_xonly_pubkey_serialize failed");
    }
    if (secp256k1_schnorrsig_sign32(
            ctx, inputs.sigs.data() + 64 * i, msg.data(), &keypair, aux.data()) != 1) {
      throw std::runtime_error("batch secp256k1_schnorrsig_sign32 failed");
    }
    if (secp256k1_schnorrsig_verify(
            ctx, inputs.sigs.data() + 64 * i, msg.data(), msg.size(), &pubkey) != 1) {
      throw std::runtime_error("batch valid Schnorr signature failed before timing");
    }
  }

  inputs.bad_sigs = inputs.sigs;
  inputs.bad_sigs[(size / 2) * 64 + 63] ^= 0x01;
  return inputs;
}

Stats divide_stats(const Stats& total, double divisor) {
  Stats per = total;
  if (!per.available || divisor <= 0.0) {
    return per;
  }
  per.mean_ns /= divisor;
  per.stddev_ns /= divisor;
  per.median_ns /= divisor;
  per.p95_ns /= divisor;
  per.p99_ns /= divisor;
  per.max_ns /= divisor;
  per.ops_per_sec *= divisor;
  return per;
}

BatchBenchmark run_experimental_batch_case(const BatchInputs& inputs, bool invalid, bool quick) {
  BatchContextPtr batch_ctx(
      qrs_exp_bip340_batch_context_create(inputs.size),
      qrs_exp_bip340_batch_context_destroy);
  if (!batch_ctx) {
    throw std::runtime_error("experimental batch context allocation failed");
  }
  const unsigned char* sigs = invalid ? inputs.bad_sigs.data() : inputs.sigs.data();
  const int expected = invalid ? 0 : 1;
  if (qrs_exp_bip340_batch_verify(
          batch_ctx.get(), inputs.size, sigs, inputs.msgs.data(), inputs.pubkeys.data()) != expected) {
    throw std::runtime_error(invalid ? "experimental invalid batch unexpectedly verified"
                                     : "experimental valid batch failed before timing");
  }

  const std::size_t samples = quick ? 5 : 21;
  const std::size_t iters =
      quick ? (inputs.size <= 512 ? 3 : 1)
            : (inputs.size <= 512 ? 12 : (inputs.size <= 2048 ? 5 : 2));
  const std::size_t warmup = quick ? 1 : 2;
  BatchBenchmark out;
  out.batch_size = inputs.size;
  out.total_verify = summarize(time_batches(
      invalid ? "schnorr_bip340_experimental_batch_invalid_total_verify"
              : "schnorr_bip340_experimental_batch_valid_total_verify",
      [&]() {
        return qrs_exp_bip340_batch_verify(
                   batch_ctx.get(), inputs.size, sigs, inputs.msgs.data(), inputs.pubkeys.data()) ==
               expected;
      },
      samples, iters, warmup));
  out.per_signature_verify =
      divide_stats(out.total_verify, static_cast<double>(inputs.size));
  return out;
}

std::vector<std::size_t> experimental_batch_sizes() {
  constexpr std::size_t block_weight_wu = 4'000'000;
  constexpr std::size_t p2tr_keypath_input_wu = 230;
  return {64, 128, 512, 2048, block_weight_wu / p2tr_keypath_input_wu};
}

std::vector<BatchBenchmark> run_experimental_batch_benchmarks(secp256k1_context* ctx,
                                                              bool invalid,
                                                              bool quick) {
  std::vector<BatchBenchmark> results;
  for (const std::size_t size : experimental_batch_sizes()) {
    BatchInputs inputs = make_batch_inputs(ctx, size);
    results.push_back(run_experimental_batch_case(inputs, invalid, quick));
  }
  return results;
}
#endif

}  // namespace

SchnorrResult run_schnorr_benchmarks(bool quick) {
  SchnorrResult r;

#if !defined(QRS_BENCH_HAVE_SECP256K1)
  r.status = "unavailable";
  r.reason =
      "libsecp256k1 is not vendored or discoverable in this package yet; add a pinned "
      "submodule/commit with schnorrsig enabled before treating this artifact as proof.";
  r.batch_reviewed_public_api_status = "unavailable";
  r.batch_reviewed_public_api_reason =
      "No reviewed libsecp256k1 batch verification API or labeled experimental batch "
      "branch is wired into this harness.";
  r.individual_valid_verify.status = "unavailable";
  r.individual_invalid_verify.status = "unavailable";
  r.batch_reviewed_public_per_signature_verify.status = "unavailable";
  r.batch_experimental_status = "unavailable";
  r.batch_experimental_reason = "libsecp256k1 is unavailable";
  r.batch_experimental_primary_per_signature_verify.status = "unavailable";
  return r;
#else
  try {
    auto ctx = create_context();
    std::array<unsigned char, 32> seckey {};
    std::array<unsigned char, 32> msg {};
    std::array<unsigned char, 32> aux {};
    fill32(seckey, 0x11);
    fill32(msg, 0x42);
    fill32(aux, 0x80);

    if (secp256k1_ec_seckey_verify(ctx.get(), seckey.data()) != 1) {
      throw std::runtime_error("deterministic test seckey failed secp256k1 validation");
    }

    secp256k1_keypair keypair;
    if (secp256k1_keypair_create(ctx.get(), &keypair, seckey.data()) != 1) {
      throw std::runtime_error("secp256k1_keypair_create failed");
    }

    secp256k1_xonly_pubkey pubkey;
    int pk_parity = 0;
    if (secp256k1_keypair_xonly_pub(ctx.get(), &pubkey, &pk_parity, &keypair) != 1) {
      throw std::runtime_error("secp256k1_keypair_xonly_pub failed");
    }
    std::array<unsigned char, 32> serialized_pubkey {};
    if (secp256k1_xonly_pubkey_serialize(ctx.get(), serialized_pubkey.data(), &pubkey) != 1) {
      throw std::runtime_error("secp256k1_xonly_pubkey_serialize failed");
    }
    secp256k1_xonly_pubkey parsed_pubkey;
    if (secp256k1_xonly_pubkey_parse(ctx.get(), &parsed_pubkey, serialized_pubkey.data()) != 1) {
      throw std::runtime_error("secp256k1_xonly_pubkey_parse failed");
    }

    std::array<unsigned char, 64> sig {};
    if (secp256k1_schnorrsig_sign32(
            ctx.get(), sig.data(), msg.data(), &keypair, aux.data()) != 1) {
      throw std::runtime_error("secp256k1_schnorrsig_sign32 failed");
    }
    if (secp256k1_schnorrsig_verify(
            ctx.get(), sig.data(), msg.data(), msg.size(), &parsed_pubkey) != 1) {
      throw std::runtime_error("valid Schnorr signature failed before timing");
    }

    std::array<unsigned char, 64> bad_sig = sig;
    bad_sig[0] ^= 0x01;
    if (secp256k1_schnorrsig_verify(
            ctx.get(), bad_sig.data(), msg.data(), msg.size(), &parsed_pubkey) != 0) {
      throw std::runtime_error("invalid fixed-length Schnorr signature verified before timing");
    }

    const std::size_t batches = quick ? 11 : 51;
    const std::size_t iters = quick ? 2000 : 20000;
    const std::size_t warmup = quick ? 200 : 1000;

    r.individual_valid_verify = summarize(time_batches(
        "schnorr_bip340_individual_valid_verify",
        [&]() {
          return secp256k1_schnorrsig_verify(
                     ctx.get(), sig.data(), msg.data(), msg.size(), &parsed_pubkey) == 1;
        },
        batches, iters, warmup));
    r.individual_invalid_verify = summarize(time_batches(
        "schnorr_bip340_individual_invalid_verify",
        [&]() {
          return secp256k1_schnorrsig_verify(
                     ctx.get(), bad_sig.data(), msg.data(), msg.size(), &parsed_pubkey) == 1;
        },
        batches, iters, warmup));

    r.status = "available";
    r.reason = "";
    r.libsecp256k1_commit = QRS_BENCH_SECP256K1_COMMIT;
    r.compile_flags =
        "bundled libsecp256k1 CMake target, schnorrsig/extrakeys enabled, tests/benchmarks disabled";
    r.batch_reviewed_public_api_status = "unavailable";
    r.batch_reviewed_public_api_reason =
        "Bundled upstream bitcoin-core/secp256k1 commit "
        QRS_BENCH_SECP256K1_COMMIT
        " exposes only the public secp256k1_schnorrsig_verify API for BIP-340; "
        "its schnorrsig tests still mark verify_batch as TODO, and git ls-remote "
        "found no upstream heads matching batch/schnorr/bip340 for a reviewed "
        "experimental baseline. No fake batch baseline is synthesized.";
    r.batch_reviewed_public_per_signature_verify.status = "unavailable";
    r.batch_experimental_valid = run_experimental_batch_benchmarks(ctx.get(), false, quick);
    r.batch_experimental_invalid = run_experimental_batch_benchmarks(ctx.get(), true, quick);
    if (r.batch_experimental_valid.empty()) {
      throw std::runtime_error("experimental batch benchmark produced no results");
    }
    r.batch_experimental_primary_per_signature_verify =
        r.batch_experimental_valid.back().per_signature_verify;
    r.batch_experimental_status = "available";
    r.batch_experimental_reason = "";
    return r;
  } catch (const std::exception& e) {
    r.status = "unavailable";
    r.reason = e.what();
    r.libsecp256k1_commit = QRS_BENCH_SECP256K1_COMMIT;
    r.individual_valid_verify.status = "unavailable";
    r.individual_invalid_verify.status = "unavailable";
    r.batch_reviewed_public_api_status = "unavailable";
    r.batch_reviewed_public_api_reason =
        "reviewed public batch baseline not run because Schnorr setup failed";
    r.batch_reviewed_public_per_signature_verify.status = "unavailable";
    r.batch_experimental_status = "unavailable";
    r.batch_experimental_reason = e.what();
    r.batch_experimental_primary_per_signature_verify.status = "unavailable";
    return r;
  }
#endif
}

}  // namespace qrs
