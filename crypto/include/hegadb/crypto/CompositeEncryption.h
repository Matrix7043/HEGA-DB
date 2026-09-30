#ifndef COMPOSITE_ENCRYPTION_H
#define COMPOSITE_ENCRYPTION_H

#include "BFVEncryption.h"
#include "CKKSEncryption.h"
#include "EncryptionWrapper.h"
#include "KeyManager.h"
#include "OpenFHEContext.h"

#include <memory>

namespace hegadb::crypto {


class CompositeEncryption final : public EncryptionWrapper {
public:
    
    CompositeEncryption();
    
    // Constructor with custom key directory
    explicit CompositeEncryption(std::string const& key_directory);

    std::unique_ptr<Ciphertext> encrypt(Plaintext const& value) override;

    Plaintext decrypt(Ciphertext const& value) override;

    OpenFHEContext const& getContext() const;

    /**
     * @brief Get the KeyManager for accessing cryptographic keys
     * 
     * This allows clients to serialize public keys for transmission to the server
     * @return Reference to the KeyManager
     */
    KeyManager const& getKeyManager() const override;
    
    // Get default key directory
    static std::string getDefaultKeyDirectory();

private:
    OpenFHEContext context_;
    KeyManager keyManager_;
    std::unique_ptr<BFVEncryption> bfvEngine_;
    std::unique_ptr<CKKSEncryption> ckksEngine_;
    
    // Initialize keys (load existing or generate new)
    void initializeKeys(std::string const& key_directory);
};

} // namespace hegadb::crypto

#endif // !COMPOSITE_ENCRYPTION_H
