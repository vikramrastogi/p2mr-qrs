#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace qrs {

struct SlhDsaKeypair {
  std::vector<unsigned char> private_key;
  std::vector<unsigned char> public_key;
};

class SlhDsaBackend {
 public:
  virtual ~SlhDsaBackend() = default;

  virtual std::string backend_name() const = 0;
  virtual std::string provider() const = 0;
  virtual std::string version() const = 0;
  virtual SlhDsaKeypair keygen() const = 0;
  virtual std::vector<unsigned char> sign(const SlhDsaKeypair& keypair,
                                          const std::vector<unsigned char>& msg) const = 0;
  virtual bool verify(const std::vector<unsigned char>& public_key,
                      const std::vector<unsigned char>& msg,
                      const std::vector<unsigned char>& sig) const = 0;
  virtual std::size_t public_key_bytes() const = 0;
  virtual std::size_t signature_bytes() const = 0;
};

}  // namespace qrs
