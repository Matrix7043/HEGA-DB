#ifndef ENCRYPTION_WRAPPER_H
#define ENCRYPTION_WRAPPER_H

#include "Ciphertext.h"
#include "Plaintext.h"

#include <memory>

namespace hegadb::crypto {

// Forward declaration
class KeyManager;

/**
 * @brief Abstract interface for encryption operations
 *
 * This interface provides encryption/decryption operations and access
 * to the KeyManager for serializing public keys to send to the server.
 */
class EncryptionWrapper {
public:
    virtual ~EncryptionWrapper() = default;

    virtual std::unique_ptr<Ciphertext> encrypt(Plaintext const& value) = 0;

    virtual Plaintext decrypt(Ciphertext const& value) = 0;

    virtual std::unique_ptr<Ciphertext> encryptInsert(Plaintext const& value) = 0;

    /**
     * @brief Get the KeyManager for accessing cryptographic keys
     *
     * This allows clients to serialize public keys for transmission to the server.
     * Only CompositeEncryption implements this; internal encryption engines throw.
     *
     * @return Const reference to the KeyManager
     * @throws std::runtime_error if called on an internal encryption engine
     */
    virtual KeyManager const& getKeyManager() const = 0;
};

}

#endif // !ENCRYPTION_WRAPPER_H
