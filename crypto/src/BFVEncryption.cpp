#include "hegadb/crypto/BFVEncryption.h"
#include "hegadb/crypto/KeyManager.h"
#include "hegadb/crypto/OpenFHECiphertext.h"

#include <stdexcept>
#include <vector>

namespace hegadb::crypto {

BFVEncryption::BFVEncryption(
    OpenFHEContext const& context,
    KeyManager const& keyManager)
    : context_(context.getBFVContext())
    , publicKey_(keyManager.getBFVPublicKey())
    , privateKey_(keyManager.getBFVPrivateKey())
{
}

std::unique_ptr<Ciphertext> BFVEncryption::encrypt(Plaintext const& value)
{
    switch (value.type()) {
    case types::DataType::Integer:
        return encryptInteger(std::get<long long>(value.value()));

    case types::DataType::String:
        return encryptString(std::get<std::string>(value.value()));

    case types::DataType::Boolean:
        return encryptBoolean(std::get<bool>(value.value()));

    case types::DataType::Float:
        throw std::runtime_error("BFVEncryption does not support Float type. Use CKKSEncryption.");

    default:
        throw std::runtime_error("Unknown data type in BFVEncryption");
    }
}

std::unique_ptr<Ciphertext> BFVEncryption::encryptInteger(long long value)
{
    // Create plaintext from integer
    lbcrypto::Plaintext plaintext = context_->MakePackedPlaintext(std::vector<int64_t> { value });

    // Encrypt using public key
    auto ciphertext = context_->Encrypt(publicKey_, plaintext);

    // Wrap in our ciphertext class with metadata
    return std::make_unique<OpenFHECiphertext>(
        ciphertext,
        SchemeType::BFV,
        types::DataType::Integer);
}

std::unique_ptr<Ciphertext> BFVEncryption::encryptString(std::string const& value)
{
    // Convert string to vector of integers (ASCII values)
    std::vector<int64_t> charValues;
    if (value.empty()) {
        charValues.push_back(0);
    } else {
        charValues.reserve(value.length());
        for (char c : value) {
            charValues.push_back(static_cast<int64_t>(static_cast<unsigned char>(c)));
        }
    }

    // Create plaintext from character values
    lbcrypto::Plaintext plaintext = context_->MakePackedPlaintext(charValues);

    // Encrypt using public key
    auto ciphertext = context_->Encrypt(publicKey_, plaintext);

    // Store original string length for decryption
    return std::make_unique<OpenFHECiphertext>(
        ciphertext,
        SchemeType::BFV,
        types::DataType::String,
        value.length());
}

std::unique_ptr<Ciphertext> BFVEncryption::encryptBoolean(bool value)
{
    // Encode boolean as 0 (false) or 1 (true)
    int64_t intValue = value ? 1 : 0;

    lbcrypto::Plaintext plaintext = context_->MakePackedPlaintext(std::vector<int64_t> { intValue });

    auto ciphertext = context_->Encrypt(publicKey_, plaintext);

    return std::make_unique<OpenFHECiphertext>(
        ciphertext,
        SchemeType::BFV,
        types::DataType::Boolean);
}

Plaintext BFVEncryption::decrypt(Ciphertext const& value)
{
    // Cast to OpenFHECiphertext
    auto const* openfheCiphertext = dynamic_cast<OpenFHECiphertext const*>(&value);

    if (!openfheCiphertext) {
        throw std::runtime_error("BFVEncryption received incompatible ciphertext type");
    }

    if (openfheCiphertext->getSchemeType() != SchemeType::BFV) {
        throw std::runtime_error("BFVEncryption received ciphertext from wrong scheme");
    }

    // Dispatch based on original type
    switch (openfheCiphertext->getOriginalType()) {
    case types::DataType::Integer:
        return decryptInteger(*openfheCiphertext);

    case types::DataType::String:
        return decryptString(*openfheCiphertext);

    case types::DataType::Boolean:
        return decryptBoolean(*openfheCiphertext);

    default:
        throw std::runtime_error("Unsupported data type in BFV decryption");
    }
}

Plaintext BFVEncryption::decryptInteger(OpenFHECiphertext const& ciphertext)
{
    lbcrypto::Plaintext plaintext;
    context_->Decrypt(privateKey_, ciphertext.getCiphertext(), &plaintext);

    // Extract the integer value
    auto values = plaintext->GetPackedValue();
    if (values.empty()) {
        throw std::runtime_error("Decrypted plaintext is empty");
    }

    long long value = values[0];
    return Plaintext(value);
}

Plaintext BFVEncryption::decryptString(OpenFHECiphertext const& ciphertext)
{
    lbcrypto::Plaintext plaintext;
    context_->Decrypt(privateKey_, ciphertext.getCiphertext(), &plaintext);

    // Extract character values
    auto values = plaintext->GetPackedValue();
    size_t length = ciphertext.getStringLength();

    if (length == 0) {
        return Plaintext(std::string(""));
    }

    if (values.size() < length) {
        throw std::runtime_error("Decrypted plaintext has fewer characters than expected");
    }

    // Convert integers back to characters
    std::string result;
    result.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        result.push_back(static_cast<char>(values[i]));
    }

    return Plaintext(result);
}

Plaintext BFVEncryption::decryptBoolean(OpenFHECiphertext const& ciphertext)
{
    lbcrypto::Plaintext plaintext;
    context_->Decrypt(privateKey_, ciphertext.getCiphertext(), &plaintext);

    auto values = plaintext->GetPackedValue();
    if (values.empty()) {
        throw std::runtime_error("Decrypted plaintext is empty");
    }

    bool value = (values[0] != 0);
    return Plaintext(value);
}

std::unique_ptr<Ciphertext> BFVEncryption::encryptInsert(Plaintext const& value)
{
    switch (value.type()) {
    case types::DataType::Integer:
        return encryptIntegerSlotZero(std::get<long long>(value.value()));
    case types::DataType::Boolean:
        return encryptBooleanSlotZero(std::get<bool>(value.value()));
    case types::DataType::String:
        return encryptStringSlotZero(std::get<std::string>(value.value()));
    default:
        throw std::runtime_error("Unsupported type in BFV encryptSingleSlot");
    }
}

std::unique_ptr<Ciphertext> BFVEncryption::encryptIntegerSlotZero(long long value)
{
    if (value < -128 || value > 127) {
        throw std::out_of_range(
            "BFVEncryption: Integer value " + std::to_string(value) + " is out of permitted range [-128, 127].");
    }

    uint32_t ring_dim = context_->GetRingDimension();
    std::vector<int64_t> slots(ring_dim, 0);
    slots[0] = value;

    lbcrypto::Plaintext plaintext = context_->MakePackedPlaintext(slots);
    auto ciphertext = context_->Encrypt(publicKey_, plaintext);

    return std::make_unique<OpenFHECiphertext>(
        ciphertext,
        SchemeType::BFV,
        types::DataType::Integer,
        0);
}

std::unique_ptr<Ciphertext> BFVEncryption::encryptBooleanSlotZero(bool value)
{
    uint32_t ring_dim = context_->GetRingDimension();
    std::vector<int64_t> slots(ring_dim, 0);
    slots[0] = value ? 1 : 0;

    lbcrypto::Plaintext plaintext = context_->MakePackedPlaintext(slots);
    auto ciphertext = context_->Encrypt(publicKey_, plaintext);

    return std::make_unique<OpenFHECiphertext>(
        ciphertext,
        SchemeType::BFV,
        types::DataType::Boolean,
        0);
}

std::unique_ptr<Ciphertext> BFVEncryption::encryptStringSlotZero(std::string const& value)
{
    uint32_t ring_dim = context_->GetRingDimension();
    std::vector<int64_t> slots(ring_dim, 0);

    size_t chars = std::min(value.size(), static_cast<size_t>(ring_dim));
    for (size_t i = 0; i < chars; ++i) {
        slots[i] = static_cast<int64_t>(static_cast<unsigned char>(value[i]));
    }

    lbcrypto::Plaintext plaintext = context_->MakePackedPlaintext(slots);
    auto ciphertext = context_->Encrypt(publicKey_, plaintext);

    return std::make_unique<OpenFHECiphertext>(
        ciphertext,
        SchemeType::BFV,
        types::DataType::String,
        value.size());
}

KeyManager const& BFVEncryption::getKeyManager() const
{
    throw std::runtime_error(
        "BFVEncryption::getKeyManager() should not be called. "
        "BFVEncryption is an internal implementation detail. "
        "Use CompositeEncryption for client-facing encryption operations.");
}

} // namespace hegadb::crypto
