#ifndef CKKS_ENCRYPTION_H
#define CKKS_ENCRYPTION_H

#include "EncryptionWrapper.h"
#include "KeyManager.h"
#include "OpenFHEContext.h"
#include "OpenFHECiphertext.h"

namespace hegadb::crypto {

/**
 * @brief CKKS scheme encryption for floating-point numbers
 *
 * This class handles encryption and decryption using the CKKS scheme,
 * which supports approximate arithmetic on real numbers.
 *
 * Note: CKKS provides approximate results, not exact values.
 * The precision is controlled by the scaling factor in the context.
 */

class CKKSEncryption final : public EncryptionWrapper {
public:

    CKKSEncryption(OpenFHEContext const& context, KeyManager const& keyManager);


    std::unique_ptr<Ciphertext> encrypt(Plaintext const& value) override;


    Plaintext decrypt(Ciphertext const& value) override;

private:
    std::unique_ptr<Ciphertext> encryptFloat(double value);
    Plaintext decryptFloat(OpenFHECiphertext const& ciphertext);

    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> context_;
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> publicKey_;
    lbcrypto::PrivateKey<lbcrypto::DCRTPoly> privateKey_;
};

} // namespace hegadb::crypto

#endif // !CKKS_ENCRYPTION_H
