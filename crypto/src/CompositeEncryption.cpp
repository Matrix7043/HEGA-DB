#include "hegadb/crypto/CompositeEncryption.h"
#include "hegadb/crypto/OpenFHECiphertext.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace hegadb::crypto {

CompositeEncryption::CompositeEncryption()
    : CompositeEncryption(getDefaultKeyDirectory())
{
}

CompositeEncryption::CompositeEncryption(std::string const& key_directory)
    : context_()
    , keyManager_(context_)  // keyManager_ holds a reference to context_
{
    context_.enableFeatures();
    // Load (or generate) keys first — loadContext() will replace context_'s
    // internal shared_ptrs in-place via the reference stored in keyManager_
    initializeKeys(key_directory);

    // Engines are created AFTER initializeKeys so they snapshot the
    // fully-loaded context and keys, not the initial empty ones
    bfvEngine_  = std::make_unique<BFVEncryption>(context_, keyManager_);
    ckksEngine_ = std::make_unique<CKKSEncryption>(context_, keyManager_);
}

void CompositeEncryption::initializeKeys(std::string const& key_directory)
{
    std::cout << "Initializing keys in directory: " << key_directory << "\n";

    try {
        if (keyManager_.keyFilesExist(key_directory)) {
            std::cout << "Found existing keys, loading...\n";
            keyManager_.loadKeys(key_directory);
            std::cout << "Loaded existing keys - consistent encryption guaranteed\n";
        } else {
            std::cout << "No existing keys found, generating new keys...\n";
            keyManager_.generateKeys();
            keyManager_.generateEvaluationKeys();

            try {
                keyManager_.saveKeys(key_directory);
                std::cout << "Generated and saved new keys to: " << key_directory << "\n";
            } catch (std::exception const& e) {
                std::cerr << "Warning: Could not save keys to disk: " << e.what() << "\n";
                std::cerr << "Keys will be regenerated on next session!\n";
            }
        }
    } catch (std::exception const& e) {
        std::cerr << "Error with persistent keys: " << e.what() << "\n";
        std::cerr << "Regenerating keys with current parameters...\n";
        keyManager_.generateKeys();
        keyManager_.generateEvaluationKeys();
        try {
            keyManager_.saveKeys(key_directory);
            std::cout << "Updated persistent keys in: " << key_directory << "\n";
        } catch (std::exception const& save_err) {
            std::cerr << "Warning: Could not save regenerated keys to disk: " << save_err.what() << "\n";
        }
    }
}

std::unique_ptr<Ciphertext> CompositeEncryption::encryptInsert(Plaintext const& value)
{
    switch (value.type()) {
    case types::DataType::Integer:
    case types::DataType::String:
    case types::DataType::Boolean:
        return bfvEngine_->encryptInsert(value);
    case types::DataType::Float:
        return ckksEngine_->encryptInsert(value);
    default:
        throw std::runtime_error("Unknown data type in CompositeEncryption::encryptSingleSlot");
    }
}

std::string CompositeEncryption::getDefaultKeyDirectory()
{
    // Use user's home directory for key storage
    std::string home = std::getenv("HOME") ? std::getenv("HOME") : ".";
    return home + "/.hegadb/keys";
}

std::unique_ptr<Ciphertext> CompositeEncryption::encrypt(Plaintext const& value)
{
    switch (value.type()) {
    case types::DataType::Integer:
    case types::DataType::String:
    case types::DataType::Boolean:

        return bfvEngine_->encrypt(value);

    case types::DataType::Float:
        return ckksEngine_->encrypt(value);

    default:
        throw std::runtime_error("Unknown data type in CompositeEncryption::encrypt");
    }
}

Plaintext CompositeEncryption::decrypt(Ciphertext const& value)
{
    // Cast to OpenFHECiphertext to read metadata
    auto const* openfheCiphertext = dynamic_cast<OpenFHECiphertext const*>(&value);

    if (!openfheCiphertext) {
        throw std::runtime_error("CompositeEncryption received non-OpenFHE ciphertext");
    }

    switch (openfheCiphertext->getSchemeType()) {
    case SchemeType::BFV:
        return bfvEngine_->decrypt(value);

    case SchemeType::CKKS:
        return ckksEngine_->decrypt(value);

    default:
        throw std::runtime_error("Unknown scheme type in CompositeEncryption::decrypt");
    }
}

OpenFHEContext const& CompositeEncryption::getContext() const
{
    return context_;
}

KeyManager const& CompositeEncryption::getKeyManager() const
{
    return keyManager_;
}

} // namespace hegadb::crypto
