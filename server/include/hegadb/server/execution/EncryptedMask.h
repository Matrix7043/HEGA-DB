#ifndef ENCRYPTED_MASK_H
#define ENCRYPTED_MASK_H

#include <hegadb/crypto/OpenFHECiphertext.h>
#include <memory>

namespace hegadb::server::execution {

/**
 * @brief How to interpret the per-slot values in the mask ciphertext.
 *
 * ComparisonEvaluator does not produce a proper 0/1 indicator bit — it
 * returns Enc(left - right) or Enc(right - left) depending on the operator.
 * This tag lets the client-side ResultDecryptor apply the correct check per
 * slot when decoding which rows matched the filter.
 *
 *  DIFF_ZERO     (EQ)  : slot[i] == 0 → row matched
 *  DIFF_NONZERO  (NEQ) : slot[i] != 0 → row matched
 *  DIFF_POSITIVE (GT)  : slot[i] >  0 → row matched  (left - right > 0)
 *  DIFF_NONNEG   (GTE) : slot[i] >= 0 → row matched
 *  DIFF_NEGATIVE (LT)  : slot[i] <  0 → row matched  (right - left stored, so slot > 0)
 *  DIFF_NONPOS   (LTE) : slot[i] <= 0 → row matched
 *  COMPOSED            : mask went through AND/OR/NOT; treat slot[i] == 0 as matched
 *                        (best-effort: proper boolean algebra requires 0/1 bits)
 *  ALL_MATCH           : no filter, every row matches
 */
enum class MaskKind {
    DIFF_ZERO,      // EQ
    DIFF_NONZERO,   // NEQ
    DIFF_POSITIVE,  // GT  (slot = left - right)
    DIFF_NONNEG,    // GTE (slot = right - left, match when >= 0 → slot <= 0 … see note)
    DIFF_NEGATIVE,  // LT  (slot = right - left, positive when right > left)
    DIFF_NONPOS,    // LTE
    COMPOSED,       // Result of AND / OR / NOT applied to sub-masks
    ALL_MATCH,      // No WHERE clause
};

class EncryptedMask {
public:
    /**
     * @brief Construct with explicit slot count and mask kind
     */
    EncryptedMask(
        crypto::OpenFHECiphertext ciphertext,
        size_t slot_count,
        MaskKind kind = MaskKind::DIFF_ZERO
    );

    /**
     * @brief Construct with automatic slot detection
     */
    explicit EncryptedMask(
        crypto::OpenFHECiphertext ciphertext,
        MaskKind kind = MaskKind::DIFF_ZERO
    );

    crypto::OpenFHECiphertext const& getCiphertext() const;
    std::string serialize() const;
    size_t getSlotCount() const;
    MaskKind getKind() const;

private:
    crypto::OpenFHECiphertext ciphertext_;
    size_t slot_count_;
    MaskKind kind_;
};

} // namespace hegadb::server::execution

#endif // ENCRYPTED_MASK_H
