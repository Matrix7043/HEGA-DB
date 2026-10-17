#ifndef MOCK_ENCRYPTION_H
#define MOCK_ENCRYPTION_H

#include "EncryptionWrapper.h"
#include <memory>

namespace hegadb::crypto {

class MockEncryption final : public EncryptionWrapper {
public:
    std::unique_ptr<Ciphertext> encrypt(Plaintext const& value) override;

    std::unique_ptr<Ciphertext> encryptInsert(Plaintext const& value) override;

    Plaintext decrypt(Ciphertext const& value) override;

    /**
     * @brief Get the KeyManager (not supported for MockEncryption)
     * @throws std::runtime_error Always throws - MockEncryption doesn't manage keys
     */
    KeyManager const& getKeyManager() const override;
};

} // namespace hegadb

#endif // !MOCK_ENCRYPTION_H
