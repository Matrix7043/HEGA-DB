#include "hegadb/crypto/BFVEncryption.h"
#include "hegadb/crypto/OpenFHECiphertext.h"
#include "hegadb/crypto/KeyManager.h"

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

std::unique_ptr<Ciphertext> BFVEncryption::encryptIntegerVector(
    std::vector<int64_t> const& values)
{
    if (values.empty()) {
        throw std::runtime_error("encryptIntegerVector: empty value vector");
    }

    lbcrypto::Plaintext plaintext = context_->MakePackedPlaintext(values);
    auto ciphertext = context_->Encrypt(publicKey_, plaintext);

    return std::make_unique<OpenFHECiphertext>(
        ciphertext,
        SchemeType::BFV,
        types::DataType::Integer,
        0
    );
}

std::unique_ptr<Ciphertext> BFVEncryption::encryptStringVector(
    std::vector<std::string> const& values,
    size_t max_string_len)
{
    if (values.empty()) {
        throw std::runtime_error("encryptStringVector: empty value vector");
    }

    // Layout: each row occupies max_string_len consecutive slots.
    // slot[i * max_string_len + j] = ASCII of values[i][j]  (0 if j >= values[i].size())
    size_t total_slots = values.size() * max_string_len;
    std::vector<int64_t> packed(total_slots, 0);

    for (size_t row = 0; row < values.size(); ++row) {
        std::string const& s = values[row];
        size_t chars = std::min(s.size(), max_string_len);
        for (size_t c = 0; c < chars; ++c) {
            packed[row * max_string_len + c] =
                static_cast<int64_t>(static_cast<unsigned char>(s[c]));
        }
    }

    lbcrypto::Plaintext plaintext = context_->MakePackedPlaintext(packed);
    auto ciphertext = context_->Encrypt(publicKey_, plaintext);

    // Encode (row_count, max_string_len) into stringLength as a combined key:
    // stringLength = row_count * 1000 + max_string_len  (works for reasonable sizes)
    size_t encoded = values.size() * 1000 + max_string_len;
    return std::make_unique<OpenFHECiphertext>(
        ciphertext,
        SchemeType::BFV,
        types::DataType::String,
        encoded
    );
}


std::unique_ptr<Ciphertext> BFVEncryption::encryptInteger(long long value)
{
    // Broadcast the scalar into all BFV slots so that per-slot subtraction
    // with a column ciphertext (which packs one row per slot) works correctly.
    // N/2 slots available in BFV packed encoding; fill them all with the same value.
    uint32_t ring_dim = context_->GetRingDimension();
    uint32_t num_slots = ring_dim;  // BFV uses all N slots
    std::vector<int64_t> slots(num_slots, value);

    lbcrypto::Plaintext plaintext = context_->MakePackedPlaintext(slots);
    auto ciphertext = context_->Encrypt(publicKey_, plaintext);

    return std::make_unique<OpenFHECiphertext>(
        ciphertext,
        SchemeType::BFV,
        types::DataType::Integer,
        0);  // stringLength unused for integers
}

std::unique_ptr<Ciphertext> BFVEncryption::encryptString(std::string const& value)
{
    // Convert string to vector of integers (ASCII values)
    std::vector<int64_t> charValues;
    if(value.empty()){
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
    // Broadcast into all slots, matching encryptInteger behaviour
    int64_t intValue = value ? 1 : 0;
    uint32_t num_slots = context_->GetRingDimension();
    std::vector<int64_t> slots(num_slots, intValue);

    lbcrypto::Plaintext plaintext = context_->MakePackedPlaintext(slots);
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

    if(length == 0){
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

KeyManager const& BFVEncryption::getKeyManager() const
{
    throw std::runtime_error(
        "BFVEncryption::getKeyManager() should not be called. "
        "BFVEncryption is an internal implementation detail. "
        "Use CompositeEncryption for client-facing encryption operations.");
}

} // namespace hegadb::crypto
