#ifndef EXECUTION_PARAMETERS_H
#define EXECUTION_PARAMETERS_H

#include <hegadb/crypto/OpenFHECiphertext.h>
#include <memory>
#include <string>
#include <vector>

namespace hegadb::server::execution{
  class ExecutionParameters {
    public:
      ExecutionParameters() = default;

      void addParameter(uint32_t index, std::string const& serialized_ciphertext);

      crypto::OpenFHECiphertext const& getParameter(uint32_t index) const;

      bool hasParameter(uint32_t index) const;

      size_t size() const;

    private:
      std::vector<std::unique_ptr<crypto::OpenFHECiphertext>> parameters_;
  };
}
#endif // EXECUTION_PARAMETERS_H
