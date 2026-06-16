#include "qrs_bench/block_model.h"
#include "qrs_bench/core_validation_model.h"
#include "qrs_bench/environment.h"
#include "qrs_bench/report_json.h"
#include "qrs_bench/report_markdown.h"
#include "qrs_bench/schnorr_secp256k1.h"
#include "qrs_bench/slh_dsa_cli.h"
#include "qrs_bench/slh_dsa_openssl.h"

#include <iostream>
#include <string>

namespace {

struct Args {
  bool quick = true;
  std::string only = "all";
  std::string json_path = "out/quick.json";
  std::string markdown_path = "out/quick.md";
};

void usage(const char* argv0) {
  std::cerr << "Usage: " << argv0
            << " [--quick|--full] [--only slh-dsa|schnorr|schnorr-batch|qrs-path|all]"
            << " --json out/result.json --markdown out/result.md\n";
}

Args parse_args(int argc, char** argv) {
  Args args;
  for (int i = 1; i < argc; ++i) {
    const std::string a = argv[i];
    if (a == "--quick") {
      args.quick = true;
    } else if (a == "--full") {
      args.quick = false;
      args.json_path = "out/full.json";
      args.markdown_path = "out/full.md";
    } else if (a == "--json" && i + 1 < argc) {
      args.json_path = argv[++i];
    } else if (a == "--markdown" && i + 1 < argc) {
      args.markdown_path = argv[++i];
    } else if (a == "--only" && i + 1 < argc) {
      args.only = argv[++i];
    } else if (a == "--help" || a == "-h") {
      usage(argv[0]);
      std::exit(0);
    } else {
      usage(argv[0]);
      throw std::runtime_error("unknown or incomplete argument: " + a);
    }
  }
  return args;
}

}  // namespace

int main(int argc, char** argv) {
  try {
    if (argc > 1 && qrs::is_slh_dsa_cli_command(argv[1])) {
      return qrs::run_slh_dsa_cli(argc, argv);
    }
    const Args args = parse_args(argc, argv);
    const bool run_slh = args.only == "all" || args.only == "slh-dsa";
    const bool run_schnorr = args.only == "all" || args.only == "schnorr" ||
                             args.only == "schnorr-batch";
    const bool run_qrs_path = args.only == "all" || args.only == "qrs-path";

    qrs::SlhDsaResult slh;
    qrs::SchnorrResult schnorr;
    qrs::QrsValidationPathResult qrs_path;
    if (run_slh) {
      slh = qrs::run_slh_dsa_benchmarks(args.quick);
    }
    if (run_schnorr) {
      schnorr = qrs::run_schnorr_benchmarks(args.quick);
    }
    if (run_qrs_path) {
      qrs_path = qrs::run_qrs_validation_path_model(args.quick);
    }

    const qrs::Stats* qrs_valid = slh.valid_verify.available ? &slh.valid_verify : nullptr;
    const qrs::Stats* qrs_invalid =
        slh.invalid_fixed_length_verify.available ? &slh.invalid_fixed_length_verify : nullptr;
    const qrs::Stats* schnorr_individual =
        schnorr.individual_valid_verify.available ? &schnorr.individual_valid_verify : nullptr;
    const qrs::Stats* schnorr_experimental_batch =
        schnorr.batch_experimental_primary_per_signature_verify.available
            ? &schnorr.batch_experimental_primary_per_signature_verify
            : nullptr;
    const qrs::Stats* schnorr_batch =
        schnorr.batch_reviewed_public_per_signature_verify.available
            ? &schnorr.batch_reviewed_public_per_signature_verify
            : nullptr;

    const auto env = qrs::collect_environment();
    const auto model =
        qrs::build_block_model(qrs_valid, qrs_invalid, schnorr_individual,
                               schnorr_experimental_batch, schnorr_batch);
    const std::string mode = args.quick ? "quick" : "full";

    qrs::write_text_file(args.json_path,
                         qrs::render_json(env, slh, schnorr, qrs_path, model, mode));
    qrs::write_text_file(args.markdown_path,
                         qrs::render_markdown(env, slh, schnorr, qrs_path, model, mode));
    std::cout << "wrote " << args.json_path << " and " << args.markdown_path << "\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "qrs_native_bench: " << e.what() << "\n";
    return 1;
  }
}
