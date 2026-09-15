#include "hegadb/client/preparation/EncryptedParameter.h"

#include <memory>
#include <utility>

namespace hegadb::client::preparation {

EncryptedParameter::EncryptedParameter(
    std::size_t index,
    std::unique_ptr<crypto::Ciphertext> ciphertext)
    : index_(index)
    , ciphertext_(std::move(ciphertext))
{
}

std::size_t EncryptedParameter::index() const { return index_; }

crypto::Ciphertext const& EncryptedParameter::ciphertext() const { return *ciphertext_; }

}
