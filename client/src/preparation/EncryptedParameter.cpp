#include "preparation/EncryptedParameter.h"

#include <memory>
#include <utility>

namespace hegadb {

EncryptedParameter::EncryptedParameter(
    std::size_t index,
    std::unique_ptr<Ciphertext> ciphertext)
    : index_(index)
    , ciphertext_(std::move(ciphertext))
{
}

std::size_t EncryptedParameter::index() const { return index_; }

Ciphertext const& EncryptedParameter::ciphertext() const { return *ciphertext_; }

}
