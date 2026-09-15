#ifndef ENCRYPTION_WRAPPER_H
#define ENCRYPTION_WRAPPER_H

#include "Ciphertext.h"
#include "Plaintext.h"

#include <memory>

namespace hegadb::crypto {

class EncryptionWrapper {
public:
    virtual ~EncryptionWrapper() = default;

    virtual std::unique_ptr<Ciphertext> encrypt(Plaintext const& value) = 0;

    virtual Plaintext decrypt(Ciphertext const& value) = 0;
};

}

#endif // !ENCRYPTION_WRAPPER_H
