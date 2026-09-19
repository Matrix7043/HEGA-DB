#ifndef BFV_ENCRYPTION_H
#define BFV_ENCRYPTION_H

#include "EncryptionWrapper.h"
#include "KeyManager.h"
#include "OpenFHEContext.h"
#include "OpenFHECiphertext.h"

namespace hegadb::crypto {

/**
 * @brief BFV scheme encryption for integers, strings, and booleans
 *
 * This class handles encryption and decryption using the BFV scheme,
 * which supports exact integer arithmetic.
 *
 * - Integers: Direct encoding
 * - Strings: Character-by-character encoding (ASCII values)
 * - Booleans: Encoded as 0 (false) or 1 (true)
 */

class BFVEncryption final : public EncryptionWrapper {
public:
    /**
     * @brief Construct BFV encryption with context and keys
     *
     * @param context The OpenFHE context manager
     * @param keyManager The key manager with generated keys
     */
    BFVEncryption(OpenFHEContext const& context, KeyManager const& keyManager);






    /**
     * @brief Encrypt a plaintext value using BFV
     *
     * Supports Integer, String, and Boolean types
     */
    std::unique_ptr<Ciphertext> encrypt(Plaintext const& value) override;






    /**
     * @brief Decrypt a ciphertext back to plaintext
     */
    Plaintext decrypt(Ciphertext const& value) override;

private:
    std::unique_ptr<Ciphertext> encryptInteger(long long value);
    std::unique_ptr<Ciphertext> encryptString(std::string const& value);
    std::unique_ptr<Ciphertext> encryptBoolean(bool value);

    Plaintext decryptInteger(OpenFHECiphertext const& ciphertext);
    Plaintext decryptString(OpenFHECiphertext const& ciphertext);
    Plaintext decryptBoolean(OpenFHECiphertext const& ciphertext);

    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> context_;
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> publicKey_;
    lbcrypto::PrivateKey<lbcrypto::DCRTPoly> privateKey_;
};

} // namespace hegadb::crypto

#endif // !BFV_ENCRYPTION_H
