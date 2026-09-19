#include "hegadb/crypto/CompositeEncryption.h"
#include "hegadb/crypto/OpenFHECiphertext.h"

#include <stdexcept>

namespace hegadb::crypto {

CompositeEncryption::CompositeEncryption()
    : context_()
    , keyManager_(context_)
{
    // Generate keys for both schemes
    keyManager_.generateKeys();
    keyManager_.generateEvaluationKeys();

    // Initialize encryption engines
    bfvEngine_ = std::make_unique<BFVEncryption>(context_, keyManager_);
    ckksEngine_ = std::make_unique<CKKSEncryption>(context_, keyManager_);
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
