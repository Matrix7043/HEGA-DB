#include <hegadb/server/execution/EncryptedMask.h>

namespace hegadb::server::execution {

EncryptedMask::EncryptedMask(
    crypto::OpenFHECiphertext ciphertext,
    size_t slot_count)
    : ciphertext_(std::move(ciphertext))
    , slot_count_(slot_count) {
}

// Overload for automatic slot detection
EncryptedMask::EncryptedMask(crypto::OpenFHECiphertext ciphertext)
    : ciphertext_(std::move(ciphertext))
    , slot_count_(0) {

    auto const& openfhe_ct = ciphertext_.getCiphertext();
    if (openfhe_ct && openfhe_ct->GetCryptoContext()) {
        auto cc = openfhe_ct->GetCryptoContext();
        uint32_t ring_dim = cc->GetRingDimension();

        slot_count_ = (ciphertext_.getSchemeType() == crypto::SchemeType::CKKS)
            ? ring_dim / 2
            : ring_dim;
    }
}

crypto::OpenFHECiphertext const& EncryptedMask::getCiphertext() const {
    return ciphertext_;
}

std::string EncryptedMask::serialize() const {
    return ciphertext_.serialize();
}

size_t EncryptedMask::getSlotCount() const {
    return slot_count_;
}

} // namespace hegadb::server::execution
