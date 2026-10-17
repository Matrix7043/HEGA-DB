#ifndef ENCRYPTED_MASK_H
#define ENCRYPTED_MASK_H

#include <hegadb/crypto/OpenFHECiphertext.h>
#include <memory>

namespace hegadb::server::execution {

/**
 * @brief Semantic tag describing how per-slot values in the mask ciphertext
 *        should be interpreted by the client-side ResultDecryptor.
 *
 * ComparisonEvaluator uses the Fermat-residue method to produce an exact
 * boolean indicator in each slot:
 *
 *   EQ(a, b)  = 1 - (a-b)^(p-1)  mod p  \u2208 {0, 1}
 *   NEQ(a, b) = (a-b)^(p-1)       mod p  \u2208 {0, 1}
 *   LT/GT/LE/GE  built from sums of EQ indicators  \u2208 {0, 1}
 *
 * The resulting slot value is always exactly 0 (no match) or 1 (match).
 * All comparison masks are tagged DIFF_NONZERO: the decryptor checks slot != 0.
 *
 * DIFF_ZERO – DIFF_NONPOS are kept for back-compat with any callers that
 * produce raw Enc(left-right) differences directly (e.g. legacy code paths).
 * COMPOSED applies after AND/OR/NOT on 0/1 masks: slot != 0 still means match.
 * ALL_MATCH : no WHERE clause, every row matches unconditionally.
 */
enum class MaskKind {
    DIFF_ZERO,      // legacy: slot = left-right, match when == 0  (EQ)
    DIFF_NONZERO,   // Fermat 0/1 indicator OR legacy != 0          (NEQ / all ops)
    DIFF_POSITIVE,  // legacy: slot = left-right, match when >  0  (GT)
    DIFF_NONNEG,    // legacy: slot = left-right, match when >= 0  (GTE)
    DIFF_NEGATIVE,  // legacy: slot = right-left, match when >  0  (LT)
    DIFF_NONPOS,    // legacy: slot = left-right, match when <= 0  (LTE)
    COMPOSED,       // Result of AND / OR / NOT on boolean 0/1 masks
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
