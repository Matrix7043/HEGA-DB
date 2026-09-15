#ifndef ENCRYPTED_PARAMETER_H
#define ENCRYPTED_PARAMETER_H

#include "crypto/Ciphertext.h"
#include <cstddef>
#include <memory>

namespace hegadb {

class EncryptedParameter {

public:
    EncryptedParameter(
        std::size_t index,
        std::unique_ptr<Ciphertext> ciphertext);

    std::size_t index() const;
    Ciphertext const& ciphertext() const;

private:
    std::size_t index_;
    std::unique_ptr<Ciphertext> ciphertext_;
};

}

#endif
