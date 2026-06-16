#include "qrs_bench/qrs_digest_model.h"

#include <openssl/sha.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace qrs {

namespace {

constexpr unsigned char kQrsLeafVersion = 0xc2;
constexpr unsigned char kQrsHashType = 0x00;
constexpr unsigned char kQrsExtFlag = 0x02;

std::string read_file(const std::string& path) {
  std::ifstream f(path);
  if (!f) {
    throw std::runtime_error("failed to open vector file: " + path);
  }
  std::ostringstream ss;
  ss << f.rdbuf();
  return ss.str();
}

std::string to_hex(const std::vector<unsigned char>& bytes) {
  std::ostringstream out;
  out << std::hex << std::setfill('0');
  for (const unsigned char b : bytes) {
    out << std::setw(2) << static_cast<unsigned int>(b);
  }
  return out.str();
}

unsigned char hex_nibble(char c) {
  if (c >= '0' && c <= '9') return static_cast<unsigned char>(c - '0');
  if (c >= 'a' && c <= 'f') return static_cast<unsigned char>(10 + c - 'a');
  throw std::runtime_error("invalid lowercase hex character");
}

std::vector<unsigned char> parse_hex(const std::string& hex, const std::string& field) {
  if ((hex.size() % 2) != 0) {
    throw std::runtime_error(field + " must be even-length hex");
  }
  std::vector<unsigned char> out(hex.size() / 2);
  for (std::size_t i = 0; i < out.size(); ++i) {
    out[i] = static_cast<unsigned char>((hex_nibble(hex[2 * i]) << 4) |
                                        hex_nibble(hex[2 * i + 1]));
  }
  return out;
}

void append_u32(std::vector<unsigned char>& out, std::uint64_t value) {
  if (value > 0xffffffffULL) {
    throw std::runtime_error("u32 out of range");
  }
  for (int i = 0; i < 4; ++i) {
    out.push_back(static_cast<unsigned char>((value >> (8 * i)) & 0xffU));
  }
}

void append_u64(std::vector<unsigned char>& out, std::uint64_t value) {
  for (int i = 0; i < 8; ++i) {
    out.push_back(static_cast<unsigned char>((value >> (8 * i)) & 0xffU));
  }
}

void append_compact_size(std::vector<unsigned char>& out, std::uint64_t value) {
  if (value < 253) {
    out.push_back(static_cast<unsigned char>(value));
  } else if (value <= 0xffffU) {
    out.push_back(0xfd);
    out.push_back(static_cast<unsigned char>(value & 0xffU));
    out.push_back(static_cast<unsigned char>((value >> 8) & 0xffU));
  } else if (value <= 0xffffffffULL) {
    out.push_back(0xfe);
    append_u32(out, value);
  } else {
    out.push_back(0xff);
    append_u64(out, value);
  }
}

std::vector<unsigned char> sha256(const std::vector<unsigned char>& payload) {
  std::vector<unsigned char> out(SHA256_DIGEST_LENGTH);
  SHA256(payload.data(), payload.size(), out.data());
  return out;
}

std::vector<unsigned char> tagged_hash(const std::string& tag,
                                       const std::vector<unsigned char>& payload) {
  const std::vector<unsigned char> tag_bytes(tag.begin(), tag.end());
  const auto tag_hash = sha256(tag_bytes);
  std::vector<unsigned char> preimage;
  preimage.reserve(tag_hash.size() * 2 + payload.size());
  preimage.insert(preimage.end(), tag_hash.begin(), tag_hash.end());
  preimage.insert(preimage.end(), tag_hash.begin(), tag_hash.end());
  preimage.insert(preimage.end(), payload.begin(), payload.end());
  return sha256(preimage);
}

std::size_t key_pos(const std::string& json, const std::string& key, std::size_t start = 0) {
  const std::string needle = "\"" + key + "\"";
  std::size_t search = start;
  while (true) {
    const std::size_t pos = json.find(needle, search);
    if (pos == std::string::npos) {
      throw std::runtime_error("missing JSON key: " + key);
    }
    std::size_t after = pos + needle.size();
    while (after < json.size() && std::isspace(static_cast<unsigned char>(json[after]))) {
      ++after;
    }
    if (after < json.size() && json[after] == ':') {
      return pos;
    }
    search = pos + needle.size();
  }
}

std::size_t value_pos(const std::string& json, const std::string& key, std::size_t start = 0) {
  std::size_t pos = key_pos(json, key, start);
  pos = json.find(':', pos);
  if (pos == std::string::npos) {
    throw std::runtime_error("missing colon after JSON key: " + key);
  }
  ++pos;
  while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) {
    ++pos;
  }
  return pos;
}

std::string parse_json_string_at(const std::string& json, std::size_t pos) {
  if (pos >= json.size() || json[pos] != '"') {
    throw std::runtime_error("expected JSON string");
  }
  ++pos;
  std::string out;
  while (pos < json.size()) {
    const char c = json[pos++];
    if (c == '"') {
      return out;
    }
    if (c == '\\') {
      throw std::runtime_error("escaped JSON strings are not supported in vectors");
    }
    out.push_back(c);
  }
  throw std::runtime_error("unterminated JSON string");
}

std::string string_value(const std::string& json, const std::string& key, std::size_t start = 0) {
  return parse_json_string_at(json, value_pos(json, key, start));
}

std::uint64_t number_value(const std::string& json,
                           const std::string& key,
                           std::uint64_t fallback,
                           bool required) {
  const std::string needle = "\"" + key + "\"";
  std::size_t search = 0;
  while (true) {
    const std::size_t key_at = json.find(needle, search);
    if (key_at == std::string::npos) {
      if (!required) return fallback;
      throw std::runtime_error("missing numeric JSON key: " + key);
    }
    std::size_t pos = key_at + needle.size();
    while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) {
      ++pos;
    }
    if (pos >= json.size() || json[pos] != ':') {
      search = key_at + needle.size();
      continue;
    }
    ++pos;
    while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) {
      ++pos;
    }
    if (pos < json.size() && std::isdigit(static_cast<unsigned char>(json[pos]))) {
      std::size_t end = pos;
      while (end < json.size() && std::isdigit(static_cast<unsigned char>(json[end]))) {
        ++end;
      }
      return std::stoull(json.substr(pos, end - pos));
    }
    search = key_at + needle.size();
  }
}

std::string object_value(const std::string& json, const std::string& key) {
  const std::size_t start = value_pos(json, key);
  if (json[start] != '{') {
    throw std::runtime_error("JSON key is not an object: " + key);
  }
  int depth = 0;
  for (std::size_t pos = start; pos < json.size(); ++pos) {
    if (json[pos] == '{') ++depth;
    if (json[pos] == '}') {
      --depth;
      if (depth == 0) {
        return json.substr(start, pos - start + 1);
      }
    }
  }
  throw std::runtime_error("unterminated JSON object for key: " + key);
}

std::string array_value(const std::string& json, const std::string& key) {
  const std::size_t start = value_pos(json, key);
  if (json[start] != '[') {
    throw std::runtime_error("JSON key is not an array: " + key);
  }
  int depth = 0;
  for (std::size_t pos = start; pos < json.size(); ++pos) {
    if (json[pos] == '[') ++depth;
    if (json[pos] == ']') {
      --depth;
      if (depth == 0) {
        return json.substr(start, pos - start + 1);
      }
    }
  }
  throw std::runtime_error("unterminated JSON array for key: " + key);
}

std::vector<unsigned char> descriptor_value(const std::string& json, const std::string& key) {
  const std::size_t pos = value_pos(json, key);
  if (json[pos] == '"') {
    return parse_hex(parse_json_string_at(json, pos), key);
  }
  if (json[pos] != '{') {
    throw std::runtime_error("descriptor value must be string or object: " + key);
  }
  const std::string obj = object_value(json, key);
  if (obj.find("\"hex\"") != std::string::npos) {
    return parse_hex(string_value(obj, "hex"), key + ".hex");
  }
  const std::string encoding = string_value(obj, "encoding");
  if (encoding == "repeat_byte") {
    const auto byte = parse_hex(string_value(obj, "byte"), key + ".byte");
    if (byte.size() != 1) {
      throw std::runtime_error(key + ".byte must be one byte");
    }
    const std::size_t count = static_cast<std::size_t>(number_value(obj, "bytes", 0, true));
    return std::vector<unsigned char>(count, byte[0]);
  }
  if (encoding == "repeat_sibling_path") {
    const auto control = parse_hex(string_value(obj, "control_byte"), key + ".control_byte");
    const auto sibling = parse_hex(string_value(obj, "sibling_hash_byte"), key + ".sibling_hash_byte");
    const std::size_t depth = static_cast<std::size_t>(number_value(obj, "depth", 0, true));
    if (control.size() != 1 || sibling.size() != 1) {
      throw std::runtime_error(key + " repeat_sibling_path requires one-byte fields");
    }
    std::vector<unsigned char> out;
    out.push_back(control[0]);
    out.insert(out.end(), depth * 32, sibling[0]);
    return out;
  }
  throw std::runtime_error("unsupported descriptor encoding for " + key + ": " + encoding);
}

struct Input {
  std::string prevout;
  std::uint64_t sequence;
};

struct Output {
  std::uint64_t value_sat;
  std::string script_pubkey;
};

std::vector<Input> parse_inputs(const std::string& json) {
  std::vector<Input> out;
  const std::regex re("\"prevout\"\\s*:\\s*\"([0-9a-f:]+)\"\\s*,\\s*\"sequence\"\\s*:\\s*([0-9]+)");
  for (std::sregex_iterator it(json.begin(), json.end(), re), end; it != end; ++it) {
    out.push_back({(*it)[1].str(), std::stoull((*it)[2].str())});
  }
  if (out.empty()) {
    throw std::runtime_error("no transaction inputs found");
  }
  return out;
}

std::vector<Output> parse_outputs(const std::string& json) {
  std::vector<Output> out;
  const std::regex re("\"value_sat\"\\s*:\\s*([0-9]+)\\s*,\\s*\"script_pubkey\"\\s*:\\s*\"([0-9a-f]+)\"");
  for (std::sregex_iterator it(json.begin(), json.end(), re), end; it != end; ++it) {
    out.push_back({std::stoull((*it)[1].str()), (*it)[2].str()});
  }
  return out;
}

std::vector<Output> parse_spent_outputs(const std::string& json,
                                        std::size_t input_count,
                                        std::uint64_t fallback_amount,
                                        const std::string& fallback_script) {
  if (json.find("\"spent_outputs\"") == std::string::npos) {
    return std::vector<Output>(input_count, {fallback_amount, fallback_script});
  }

  const std::string array = array_value(json, "spent_outputs");
  std::vector<Output> out;
  const std::regex re("\"amount_sats\"\\s*:\\s*([0-9]+)\\s*,\\s*\"script_pubkey\"\\s*:\\s*\"([0-9a-f]+)\"");
  for (std::sregex_iterator it(array.begin(), array.end(), re), end; it != end; ++it) {
    out.push_back({std::stoull((*it)[1].str()), (*it)[2].str()});
  }
  if (out.empty()) {
    throw std::runtime_error("spent_outputs must be non-empty when present");
  }
  if (out.size() != input_count) {
    throw std::runtime_error("spent_outputs length must match transaction input count");
  }
  return out;
}

std::vector<unsigned char> parse_prevout(const std::string& prevout) {
  const std::size_t sep = prevout.find(':');
  if (sep == std::string::npos) {
    throw std::runtime_error("prevout must be hex:index");
  }
  auto txid = parse_hex(prevout.substr(0, sep), "prevout txid");
  if (txid.size() == 1) {
    txid = std::vector<unsigned char>(32, txid[0]);
  }
  if (txid.size() != 32) {
    throw std::runtime_error("prevout txid must be 32 bytes or one-byte shorthand");
  }
  append_u32(txid, std::stoull(prevout.substr(sep + 1)));
  return txid;
}

std::vector<unsigned char> serialize_script(const std::string& hex) {
  const auto script = parse_hex(hex, "script_pubkey");
  std::vector<unsigned char> out;
  append_compact_size(out, script.size());
  out.insert(out.end(), script.begin(), script.end());
  return out;
}

std::vector<unsigned char> leaf_hash(const std::vector<unsigned char>& pubkey) {
  std::vector<unsigned char> payload;
  payload.push_back(kQrsLeafVersion);
  append_compact_size(payload, pubkey.size());
  payload.insert(payload.end(), pubkey.begin(), pubkey.end());
  return tagged_hash("TapLeaf", payload);
}

std::vector<unsigned char> merkle_root_from_control(std::vector<unsigned char> root,
                                                    const std::vector<unsigned char>& control) {
  for (std::size_t offset = 1; offset < control.size(); offset += 32) {
    const std::size_t end = std::min(offset + 32, control.size());
    std::vector<unsigned char> sibling(control.begin() + offset, control.begin() + end);
    std::vector<unsigned char> payload;
    if (root < sibling) {
      payload.insert(payload.end(), root.begin(), root.end());
      payload.insert(payload.end(), sibling.begin(), sibling.end());
    } else {
      payload.insert(payload.end(), sibling.begin(), sibling.end());
      payload.insert(payload.end(), root.begin(), root.end());
    }
    root = tagged_hash("TapBranch", payload);
  }
  return root;
}

QrsDigestVectorResult compute_from_json(const std::string& json) {
  const auto pubkey = descriptor_value(json, "qrs_public_key");
  const auto control = descriptor_value(json, "control_block");
  const std::string annex_hex =
      json.find("\"annex\"") == std::string::npos ? "" : string_value(json, "annex");
  const auto annex = parse_hex(annex_hex, "annex");
  const auto inputs = parse_inputs(json);
  const auto outputs = parse_outputs(json);
  const std::uint64_t tx_version = number_value(json, "version", 0, true);
  const std::uint64_t lock_time = number_value(json, "lock_time", 0, true);
  const std::uint64_t input_index = number_value(json, "input_index", 0, false);
  const std::string spent_script = string_value(json, "spent_output_scriptPubKey");
  const std::uint64_t fallback_amount =
      outputs.empty() ? 0 : outputs.front().value_sat;
  const auto spent = parse_spent_outputs(json, inputs.size(), fallback_amount, spent_script);

  std::vector<unsigned char> prevouts_payload;
  std::vector<unsigned char> amounts_payload;
  std::vector<unsigned char> scriptpubkeys_payload;
  std::vector<unsigned char> sequences_payload;
  for (std::size_t i = 0; i < inputs.size(); ++i) {
    const auto prevout = parse_prevout(inputs[i].prevout);
    prevouts_payload.insert(prevouts_payload.end(), prevout.begin(), prevout.end());
    append_u64(amounts_payload, spent[i].value_sat);
    const auto script = serialize_script(spent[i].script_pubkey);
    scriptpubkeys_payload.insert(scriptpubkeys_payload.end(), script.begin(), script.end());
    append_u32(sequences_payload, inputs[i].sequence);
  }

  std::vector<unsigned char> outputs_payload;
  for (const auto& output : outputs) {
    append_u64(outputs_payload, output.value_sat);
    const auto script = serialize_script(output.script_pubkey);
    outputs_payload.insert(outputs_payload.end(), script.begin(), script.end());
  }

  const auto leaf = leaf_hash(pubkey);
  const auto root = merkle_root_from_control(leaf, control);
  const unsigned char annex_present = annex.empty() ? 0 : 1;
  const unsigned char spend_type = static_cast<unsigned char>(kQrsExtFlag * 2 + annex_present);
  std::vector<unsigned char> sigmsg;
  sigmsg.push_back(kQrsHashType);
  sigmsg.push_back(spend_type);
  append_u32(sigmsg, tx_version);
  append_u32(sigmsg, lock_time);
  const auto h_prevouts = sha256(prevouts_payload);
  const auto h_amounts = sha256(amounts_payload);
  const auto h_scripts = sha256(scriptpubkeys_payload);
  const auto h_sequences = sha256(sequences_payload);
  const auto h_outputs = sha256(outputs_payload);
  sigmsg.insert(sigmsg.end(), h_prevouts.begin(), h_prevouts.end());
  sigmsg.insert(sigmsg.end(), h_amounts.begin(), h_amounts.end());
  sigmsg.insert(sigmsg.end(), h_scripts.begin(), h_scripts.end());
  sigmsg.insert(sigmsg.end(), h_sequences.begin(), h_sequences.end());
  sigmsg.insert(sigmsg.end(), h_outputs.begin(), h_outputs.end());
  append_u32(sigmsg, input_index);
  if (!annex.empty()) {
    std::vector<unsigned char> annex_payload;
    append_compact_size(annex_payload, annex.size());
    annex_payload.insert(annex_payload.end(), annex.begin(), annex.end());
    const auto h_annex = sha256(annex_payload);
    sigmsg.insert(sigmsg.end(), h_annex.begin(), h_annex.end());
  }
  sigmsg.insert(sigmsg.end(), leaf.begin(), leaf.end());
  std::vector<unsigned char> sighash_payload;
  sighash_payload.push_back(0x00);
  sighash_payload.insert(sighash_payload.end(), sigmsg.begin(), sigmsg.end());
  const auto msg = tagged_hash("TapSighash", sighash_payload);

  const bool reaches = json.find("\"reaches_slh_dsa_verifier\": true") != std::string::npos;
  return {to_hex(leaf), to_hex(root), to_hex(msg), reaches};
}

}  // namespace

QrsDigestVectorResult compute_qrs_digest_vector_file(const std::string& path) {
  return compute_from_json(read_file(path));
}

bool is_qrs_digest_cli_command(const std::string& command) {
  return command == "--qrs-digest-vector";
}

int run_qrs_digest_cli(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "usage: " << argv[0] << " --qrs-digest-vector test_vectors/qrs_valid_no_annex.json\n";
    return 3;
  }
  try {
    const auto result = compute_qrs_digest_vector_file(argv[2]);
    std::cout << "{\n"
              << "  \"leaf_hash\": \"" << result.leaf_hash << "\",\n"
              << "  \"merkle_root\": \"" << result.merkle_root << "\",\n"
              << "  \"qrs_msg\": \"" << result.qrs_msg << "\",\n"
              << "  \"reaches_slh_dsa_verifier\": "
              << (result.reaches_slh_dsa_verifier ? "true" : "false") << "\n"
              << "}\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "qrs-digest-vector unavailable: " << e.what() << "\n";
    return 2;
  }
}

}  // namespace qrs
