#ifndef ENCRYPTED_MASK_H
#define ENCRYPTED_MASK_H

#include <hegadb/crypto/OpenFHECiphertext.h>
#include <memory>

namespace hegadb::server::execution {

class EncryptedMask {
public:
    /**
     * @brief Construct with explicit slot count
     */
    EncryptedMask(
        crypto::OpenFHECiphertext ciphertext,
        size_t slot_count
    );

    /**
     * @brief Construct with automatic slot detection
     */
    explicit EncryptedMask(crypto::OpenFHECiphertext ciphertext);

    crypto::OpenFHECiphertext const& getCiphertext() const;
    std::string serialize() const;
    size_t getSlotCount() const;

private:
    crypto::OpenFHECiphertext ciphertext_;
    size_t slot_count_;
};

} // namespace hegadb::server::execution

#endif // ENCRYPTED_MASK_H
