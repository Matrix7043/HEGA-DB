#ifndef ENCRYPTED_PARAMETER_H
#define ENCRYPTED_PARAMETER_H

#include "hegadb/crypto/Ciphertext.h"
#include <cstddef>
#include <memory>

namespace hegadb::client::preparation {

class EncryptedParameter {

public:
    EncryptedParameter(
        std::size_t index,
        std::unique_ptr<crypto::Ciphertext> ciphertext);

    std::size_t index() const;
    crypto::Ciphertext const& ciphertext() const;

private:
    std::size_t index_;
    std::unique_ptr<crypto::Ciphertext> ciphertext_;
};

}

#endif
