#include <hegadb/server/execution/EncryptedMask.h>

namespace hegadb::server::execution {

EncryptedMask::EncryptedMask(
    crypto::OpenFHECiphertext ciphertext,
    size_t slot_count,
    MaskKind kind)
    : ciphertext_(std::move(ciphertext))
    , slot_count_(slot_count)
    , kind_(kind) {
}

EncryptedMask::EncryptedMask(
    crypto::OpenFHECiphertext ciphertext,
    MaskKind kind)
    : ciphertext_(std::move(ciphertext))
    , slot_count_(0)
    , kind_(kind) {

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

MaskKind EncryptedMask::getKind() const {
    return kind_;
}

} // namespace hegadb::server::execution
