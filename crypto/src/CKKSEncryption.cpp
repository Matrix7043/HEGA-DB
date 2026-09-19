#include "hegadb/crypto/CKKSEncryption.h"
#include "hegadb/crypto/OpenFHECiphertext.h"

#include <stdexcept>
#include <vector>

namespace hegadb::crypto {

CKKSEncryption::CKKSEncryption(
    OpenFHEContext const& context,
    KeyManager const& keyManager)
    : context_(context.getCKKSContext())
    , publicKey_(keyManager.getCKKSPublicKey())
    , privateKey_(keyManager.getCKKSPrivateKey())
{
}

std::unique_ptr<Ciphertext> CKKSEncryption::encrypt(Plaintext const& value)
{
    if (value.type() != types::DataType::Float) {
        throw std::runtime_error("CKKSEncryption only supports Float type");
    }

    // Note: Your Variant doesn't include double/float yet
    // This assumes you'll add it or we'll need to convert from string
    // For now, I'll add a note that this needs the Variant to support float

    // TODO: Once Variant supports double, use:
    // double floatValue = std::get<double>(value.value());

    // Temporary workaround: throw error until Variant is updated
    throw std::runtime_error(
        "Float type not yet supported in Variant. "
        "Please add 'double' to types::Variant in Variant.h");

    // Uncomment when Variant is updated:
    // return encryptFloat(floatValue);
}

std::unique_ptr<Ciphertext> CKKSEncryption::encryptFloat(double value)
{
    // Create plaintext from double value
    std::vector<double> values = {value};
    lbcrypto::Plaintext plaintext = context_->MakeCKKSPackedPlaintext(values);

    // Encrypt using public key
    auto ciphertext = context_->Encrypt(publicKey_, plaintext);

    return std::make_unique<OpenFHECiphertext>(
        ciphertext,
        SchemeType::CKKS,
        types::DataType::Float);
}

Plaintext CKKSEncryption::decrypt(Ciphertext const& value)
{
    auto const* openfheCiphertext = dynamic_cast<OpenFHECiphertext const*>(&value);

    if (!openfheCiphertext) {
        throw std::runtime_error("CKKSEncryption received incompatible ciphertext type");
    }

    if (openfheCiphertext->getSchemeType() != SchemeType::CKKS) {
        throw std::runtime_error("CKKSEncryption received ciphertext from wrong scheme");
    }

    if (openfheCiphertext->getOriginalType() != types::DataType::Float) {
        throw std::runtime_error("CKKSEncryption expected Float type");
    }

    return decryptFloat(*openfheCiphertext);
}

Plaintext CKKSEncryption::decryptFloat(OpenFHECiphertext const& ciphertext)
{
    lbcrypto::Plaintext plaintext;
    context_->Decrypt(privateKey_, ciphertext.getCiphertext(), &plaintext);

    // Extract the real part of the complex values
    auto values = plaintext->GetRealPackedValue();
    if (values.empty()) {
        throw std::runtime_error("Decrypted plaintext is empty");
    }

    double value = values[0];

    // TODO: Once Variant supports double, return:
    // return Plaintext(value);

    // Temporary: throw error
    throw std::runtime_error("Float type not yet supported in Variant");
}

} // namespace hegadb::crypto
