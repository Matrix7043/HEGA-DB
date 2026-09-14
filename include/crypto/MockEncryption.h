#ifndef MOCK_ENCRYPTION_H
#define MOCK_ENCRYPTION_H

#include "crypto/EncryptionWrapper.h"
#include <memory>

namespace hegadb {

class MockEncryption final : public EncryptionWrapper {
public:
    std::unique_ptr<Ciphertext> encrypt(Plaintext const& value) override;

    Plaintext decrypt(Ciphertext const& value) override;
};

} // namespace hegadb

#endif // !MOCK_ENCRYPTION_H
