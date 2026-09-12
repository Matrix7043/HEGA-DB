#ifndef ENCRYPTION_WRAPPER_H
#define ENCRYPTION_WRAPPER_H

#include "crypto/Ciphertext.h"
#include "crypto/Plaintext.h"

namespace hegadb {

class EncryptionWrapper {
public:
    virtual ~EncryptionWrapper() = default;

    virtual Ciphertext encrypt(Plaintext const& value) = 0;

    virtual Plaintext decrypt(Ciphertext const& value) = 0;
};

}

#endif // !ENCRYPTION_WRAPPER_H
