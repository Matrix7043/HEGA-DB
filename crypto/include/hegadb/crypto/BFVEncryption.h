#ifndef BFV_ENCRYPTION_H
#define BFV_ENCRYPTION_H

#include "EncryptionWrapper.h"
#include "KeyManager.h"
#include "OpenFHECiphertext.h"
#include "OpenFHEContext.h"

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

    std::unique_ptr<Ciphertext> encryptInsert(Plaintext const& value) override;

    /**
     * @brief Encrypt a plaintext value using BFV
     *
     * Supports Integer, String, and Boolean types
     */
    std::unique_ptr<Ciphertext> encrypt(Plaintext const& value) override;

    /**
     * @brief Pack and encrypt a vector of integers into a single BFV ciphertext.
     *
     * Uses BFV slot packing so that the i-th slot holds values[i].
     * This is the correct way to store a whole database column.
     */
    std::unique_ptr<Ciphertext> encryptIntegerVector(std::vector<int64_t> const& values);

    /**
     * @brief Pack and encrypt a vector of strings into a single BFV ciphertext.
     *
     * Encodes each string as its first character's ASCII value in slot i.
     * For short columns (<=50 chars) the full string is stored in consecutive slots
     * with a stride of max_string_len.
     * Returns the ciphertext and stores row_count + max_string_len in metadata.
     */
    std::unique_ptr<Ciphertext> encryptStringVector(
        std::vector<std::string> const& values,
        size_t max_string_len = 32);






    /**
     * @brief Decrypt a ciphertext back to plaintext
     */
    Plaintext decrypt(Ciphertext const& value) override;

    /**
     * @brief Get the KeyManager (not supported for internal encryption engines)
     * @throws std::runtime_error Always throws - BFVEncryption is an internal implementation detail
     */
    KeyManager const& getKeyManager() const override;

private:
    std::unique_ptr<Ciphertext> encryptInteger(long long value);
    std::unique_ptr<Ciphertext> encryptString(std::string const& value);
    std::unique_ptr<Ciphertext> encryptBoolean(bool value);

    Plaintext decryptInteger(OpenFHECiphertext const& ciphertext);
    Plaintext decryptString(OpenFHECiphertext const& ciphertext);
    Plaintext decryptBoolean(OpenFHECiphertext const& ciphertext);

    std::unique_ptr<Ciphertext> encryptIntegerSlotZero(long long value);
    std::unique_ptr<Ciphertext> encryptBooleanSlotZero(bool value);
    std::unique_ptr<Ciphertext> encryptStringSlotZero(std::string const& value);

    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> context_;
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> publicKey_;
    lbcrypto::PrivateKey<lbcrypto::DCRTPoly> privateKey_;
};

} // namespace hegadb::crypto

#endif // !BFV_ENCRYPTION_H
