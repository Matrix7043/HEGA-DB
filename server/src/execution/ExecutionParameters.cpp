#include <hegadb/server/execution/ExecutionParameters.h>
#include <stdexcept>

namespace hegadb::server::execution {
  void ExecutionParameters::addParameter(
      uint32_t index,
      std::string const& serialized_ciphertext){
    if (index >= parameters_.size()){
      parameters_.resize(index + 1);
    }

    parameters_[index] = std::make_unique<crypto::OpenFHECiphertext>(
        crypto::OpenFHECiphertext::deserialize(serialized_ciphertext)
        );
  }

  crypto::OpenFHECiphertext const& ExecutionParameters::getParameter(uint32_t index) const{
    if (index >= parameters_.size() || !parameters_[index]){
      throw std::out_of_range(
          "Parameter index " + std::to_string(index) + " not found"
          );
    }

    return *parameters_[index];
  }

  bool ExecutionParameters::hasParameter(uint32_t index) const {
    return index < parameters_.size() && parameters_[index] != nullptr;
  }

  size_t ExecutionParameters::size() const {
    return parameters_.size();
  }
}
