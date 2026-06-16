#include "qrs_bench/report_markdown.h"

#include <iomanip>
#include <sstream>

namespace qrs {

namespace {

void stats_row(std::ostringstream& o, const std::string& name, const Stats& s) {
  if (!s.available) {
    o << "| " << name << " | " << s.status << " | | | | | |\n";
    return;
  }
  o << "| " << name << " | available | " << s.median_ns << " | " << s.p95_ns
    << " | " << s.p99_ns << " | " << s.max_ns << " | " << s.ops_per_sec << " |\n";
}

void batch_row(std::ostringstream& o, const BatchBenchmark& b) {
  if (!b.total_verify.available || !b.per_signature_verify.available) {
    o << "| " << b.batch_size << " | unavailable | | | | |\n";
    return;
  }
  o << "| " << b.batch_size << " | available | " << b.total_verify.median_ns
    << " | " << b.total_verify.p99_ns << " | " << b.per_signature_verify.median_ns
    << " | " << b.per_signature_verify.p99_ns << " |\n";
}

void block_row(std::ostringstream& o, const std::string& name, const BlockEstimate& e) {
  if (!e.available) {
    o << "| " << name << " | " << e.status << " | " << e.max_inputs << " | | | | |\n";
    return;
  }
  o << "| " << name << " | available | " << e.max_inputs << " | "
    << e.ms.at("median_ms") << " | " << e.ms.at("p95_ms") << " | "
    << e.ms.at("p99_ms") << " | " << e.ms.at("max_ms") << " |\n";
}

std::string conclusion(const SlhDsaResult& slh,
                       const SchnorrResult& schnorr,
                       const BlockModel& model) {
  if (!slh.valid_verify.available || !slh.invalid_fixed_length_verify.available) {
    return "The run does not contain usable QRS SLH-DSA timings, so it cannot support the proposed no-additional-budget rule.";
  }
  if (!schnorr.individual_valid_verify.available) {
    return "The run contains QRS timings, but the individual Schnorr baseline is unavailable; it is not a proof package result yet.";
  }
  if (!schnorr.batch_reviewed_public_per_signature_verify.available) {
    if (schnorr.batch_experimental_primary_per_signature_verify.available) {
      return "The run contains QRS, individual Schnorr, and experimental batch Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should treat the experimental batch result as sensitivity analysis, not consensus proof.";
    }
    return "The run contains QRS and individual Schnorr timings, but the reviewed public batch Schnorr baseline is unavailable; reviewers should not treat this as closing the resource-accounting question.";
  }

  const double qrs_p99 = model.qrs_saturated_block_valid.ms.at("p99_ms");
  const double schnorr_ind_p99 = model.schnorr_individual_saturated_block.ms.at("p99_ms");
  const double schnorr_batch_p99 = model.schnorr_batch_saturated_block.ms.at("p99_ms");
  if (qrs_p99 <= schnorr_ind_p99 && qrs_p99 <= schnorr_batch_p99) {
    return "On this machine and with these library versions, the result is consistent with the no-additional-budget hypothesis, subject to broader native validation-cost review.";
  }
  if (qrs_p99 <= schnorr_ind_p99 && qrs_p99 > schnorr_batch_p99) {
    return "On this machine, QRS exceeds the batch Schnorr baseline; reviewers should decide whether an explicit QRS budget is required.";
  }
  return "On this machine, QRS exceeds the individual Schnorr baseline; this result suggests an explicit QRS validation budget is required.";
}

}  // namespace

std::string render_markdown(const Environment& env,
                            const SlhDsaResult& slh,
                            const SchnorrResult& schnorr,
                            const QrsValidationPathResult& qrs_path,
                            const BlockModel& block_model,
                            const std::string& mode) {
  std::ostringstream o;
  o << std::fixed << std::setprecision(3);
  o << "# QRS Native Benchmark Report\n\n";
  o << "Mode: `" << mode << "`\n\n";
  o << "## Conclusion\n\n";
  o << conclusion(slh, schnorr, block_model) << "\n\n";
  o << "## Environment\n\n";
  for (const auto& [k, v] : env.fields) {
    o << "- `" << k << "`: " << v << "\n";
  }
  o << "- `OpenSSL`: " << slh.openssl_version << "\n";
  o << "- `OpenSSL provider`: " << slh.provider << "\n";
  o << "- `SLH-DSA mode`: " << slh.mode << "\n";
  o << "- `SLH-DSA public key bytes`: " << slh.public_key_bytes << "\n";
  o << "- `SLH-DSA signature bytes`: " << slh.signature_bytes << "\n";
  o << "- `libsecp256k1`: " << schnorr.libsecp256k1_commit << "\n\n";

  o << "## SLH-DSA Backend Status\n\n";
  o << "| Backend | Status | Provider | Version | Public key bytes | Signature bytes | Reason |\n";
  o << "|---|---:|---|---|---:|---:|---|\n";
  for (const auto& backend : slh.backends) {
    o << "| " << backend.name << " | " << backend.status << " | " << backend.provider
      << " | " << backend.version << " | " << backend.public_key_bytes << " | "
      << backend.signature_bytes << " | " << backend.reason << " |\n";
  }
  o << "\nThe second reviewed SLH-DSA backend is reported separately from the OpenSSL "
       "backend; it remains unavailable until a reviewed/pinned verifier strategy "
       "is wired for cross-backend agreement.\n\n";

  o << "## Raw Timing Table\n\n";
  o << "Timing statistics are computed as the median of per-batch means, p95 of "
       "per-batch means, p99 of per-batch means, and max of per-batch means; "
       "they are not individual operation latency samples.\n\n";
  o << "| Benchmark | Status | Median batch-mean ns | p95 batch-mean ns | "
       "p99 batch-mean ns | Max batch-mean ns | Ops/sec |\n";
  o << "|---|---:|---:|---:|---:|---:|---:|\n";
  stats_row(o, "SLH-DSA-SHA2-128s valid verify", slh.valid_verify);
  stats_row(o, "SLH-DSA-SHA2-128s invalid fixed-length verify",
            slh.invalid_fixed_length_verify);
  stats_row(o, "BIP-340 Schnorr individual valid verify", schnorr.individual_valid_verify);
  stats_row(o, "BIP-340 Schnorr individual invalid verify", schnorr.individual_invalid_verify);
  stats_row(o, "BIP-340 Schnorr reviewed-public batch per-signature verify",
            schnorr.batch_reviewed_public_per_signature_verify);
  stats_row(o, "BIP-340 Schnorr experimental batch primary per-signature verify",
            schnorr.batch_experimental_primary_per_signature_verify);

  o << "\n## SLH-DSA Invalid Fixed-Length Suite\n\n";
  o << "Each case keeps the signature length at 7,856 bytes and is checked to fail "
       "before timing. The compatibility `invalid_fixed_length_verify` value in "
       "JSON is the worst observed p99 case from this suite.\n\n";
  o << "| Case | Status | Median batch-mean ns | p95 batch-mean ns | "
       "p99 batch-mean ns | Max batch-mean ns | Ops/sec |\n";
  o << "|---|---:|---:|---:|---:|---:|---:|\n";
  for (const auto& c : slh.invalid_fixed_length_cases) {
    stats_row(o, c.name, c.stats);
  }
  stats_row(o, "min observed invalid fixed-length", slh.invalid_fixed_length_min_observed);
  stats_row(o, "best observed invalid fixed-length", slh.invalid_fixed_length_best_observed);
  stats_row(o, "median observed invalid fixed-length", slh.invalid_fixed_length_median_observed);
  stats_row(o, "p99 observed invalid fixed-length", slh.invalid_fixed_length_p99_observed);
  stats_row(o, "worst observed invalid fixed-length", slh.invalid_fixed_length_worst_observed);
  o << "\nWorst observed invalid fixed-length case: `"
    << slh.invalid_fixed_length_case_name_worst << "`.\n";

  o << "\n## QRS Validation Path Model\n\n";
  o << "- `status`: " << qrs_path.status << "\n";
  o << "- `scope`: " << qrs_path.scope << "\n";
  o << "- `qrs_msg_scope`: " << qrs_path.qrs_msg_scope << "\n\n";
  o << "| Bucket | Status | Median batch-mean ns | p95 batch-mean ns | "
       "p99 batch-mean ns | Max batch-mean ns | Ops/sec |\n";
  o << "|---|---:|---:|---:|---:|---:|---:|\n";
  stats_row(o, "structural checks", qrs_path.structural_checks);
  stats_row(o, "qrs_msg construction", qrs_path.qrs_msg_construction);
  stats_row(o, "verifier valid call", qrs_path.verifier_valid);
  stats_row(o, "total modeled valid path", qrs_path.total_valid);
  stats_row(o, "invalid structural reject", qrs_path.invalid_structural);
  stats_row(o, "invalid fixed-length crypto reject", qrs_path.invalid_fixed_length);

  o << "\n## Batch Schnorr Baseline\n\n";
  o << "- `reviewed_public_api_status`: " << schnorr.batch_reviewed_public_api_status << "\n";
  o << "- `reviewed_public_api_reason`: " << schnorr.batch_reviewed_public_api_reason << "\n";
  o << "- `experimental_status`: " << schnorr.batch_experimental_status << "\n";
  o << "- `experimental_label`: " << schnorr.batch_experimental_label << "\n";
  o << "- BIP-340 challenge self-test: `"
    << schnorr.batch_experimental_challenge_self_test_status << "` - "
    << schnorr.batch_experimental_challenge_self_test_reason << "\n";
  o << "- `coefficient_policy`: " << schnorr.batch_experimental_coefficient_policy << "\n";
  o << "- `invalid_batch_policy`: " << schnorr.batch_experimental_invalid_policy << "\n";
  o << "- `timing_scope`: " << schnorr.batch_experimental_timing_scope << "\n";
  o << "- `policy`: no assumed speedup is applied; without a reviewed public batch "
       "Schnorr implementation, the batch baseline remains unavailable.\n";
  o << "\nReviewed public libsecp256k1 batch API: "
    << schnorr.batch_reviewed_public_api_status << ".\n";
  o << "Experimental native BIP-340 batch baseline: "
    << schnorr.batch_experimental_status << ".\n";
  o << "This benchmark implements the experimental native BIP-340 batch baseline "
       "for sensitivity analysis only; it is not a reviewed public libsecp256k1 "
       "API and must not be treated as consensus-grade validation code.\n";

  o << "\n## Experimental Batch Timing\n\n";
  o << "| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | "
       "Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |\n";
  o << "|---:|---:|---:|---:|---:|---:|\n";
  for (const auto& b : schnorr.batch_experimental_valid) {
    batch_row(o, b);
  }

  o << "\n## Experimental Invalid Batch Timing\n\n";
  o << "| Batch size | Status | Total median batch-mean ns | Total p99 batch-mean ns | "
       "Per-signature median batch-mean ns | Per-signature p99 batch-mean ns |\n";
  o << "|---:|---:|---:|---:|---:|---:|\n";
  for (const auto& b : schnorr.batch_experimental_invalid) {
    batch_row(o, b);
  }

  o << "\n## Block Model\n\n";
  o << "Simplified per-input saturation approximation. Real blocks include transaction "
       "overhead and outputs.\n\n";
  o << "| Model | Status | Max inputs | Median ms | p95 ms | p99 ms | Max ms |\n";
  o << "|---|---:|---:|---:|---:|---:|---:|\n";
  block_row(o, "QRS valid saturated block", block_model.qrs_saturated_block_valid);
  block_row(o, "QRS invalid fixed-length saturated block",
            block_model.qrs_saturated_block_invalid_fixed_length);
  block_row(o, "Schnorr individual saturated block",
            block_model.schnorr_individual_saturated_block);
  block_row(o, "Schnorr experimental batch saturated block",
            block_model.schnorr_experimental_batch_saturated_block);
  block_row(o, "Schnorr reviewed-public batch saturated block",
            block_model.schnorr_batch_saturated_block);

  o << "\n## QRS vs experimental batch Schnorr\n\n";
  if (block_model.qrs_saturated_block_valid.available &&
      block_model.schnorr_experimental_batch_saturated_block.available) {
    o << "- `qrs_valid_p99_ms`: "
      << block_model.qrs_saturated_block_valid.ms.at("p99_ms") << "\n";
    o << "- `experimental_batch_schnorr_p99_ms`: "
      << block_model.schnorr_experimental_batch_saturated_block.ms.at("p99_ms") << "\n";
  } else {
    o << "- unavailable\n";
  }

  o << "\n## Caveats\n\n";
  o << "- One machine is not consensus readiness evidence by itself.\n";
  o << "- Frequency scaling is reported but not controlled by the harness.\n";
  o << "- QRS timings are only meaningful when `SLH-DSA-SHA2-128s` is available from OpenSSL.\n";
  o << "- Batch Schnorr must be labeled unavailable unless a reviewed batch API or reviewed experimental branch is supplied.\n";
  o << "- The experimental batch Schnorr baseline uses local benchmark code and libsecp256k1 internals; it is sensitivity analysis, not reviewed consensus code.\n";
  o << "- The QRS validation path section is a model of validation buckets, not Bitcoin Core consensus integration.\n";
  o << "- The block model is deliberately transparent and simplified; add realistic transaction-shape models separately if needed.\n";
  return o.str();
}

}  // namespace qrs
