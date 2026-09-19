#ifndef OPENFHE_CIPHERTEXT_H
#define OPENFHE_CIPHERTEXT_H

#include "Ciphertext.h"
#include "openfhe.h"
#include <hegadb/types/DataType.h>

namespace hegadb::crypto {

enum class SchemeType {
    BFV,  // For integers, strings, booleans
    CKKS  // For floats
};

/**
 * Wrapper around OpenFHE ciphertext with metadata
 *
 * This class wraps the OpenFHE ciphertext and stores additional
 * information needed for proper decryption and type handling.
 */


class OpenFHECiphertext final : public Ciphertext {
public:
    /**
     * @brief Construct a new OpenFHE ciphertext wrapper
     *
     * @param ciphertext The OpenFHE ciphertext object
     * @param schemeType Which scheme was used (BFV or CKKS)
     * @param originalType The original data type before encryption
     * @param stringLength For strings, the original length (0 otherwise)
     */



    OpenFHECiphertext(
        lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ciphertext,
        SchemeType schemeType,
        types::DataType originalType,
        size_t stringLength = 0);


    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> getCiphertext() const;

    /**
     *  Get the scheme type used for this ciphertext
     */

    SchemeType getSchemeType() const;

    /**
     * Get the original data type before encryption
     */
    types::DataType getOriginalType() const;

    /**
     * Get the string length (only valid for string types)
     */
    size_t getStringLength() const;

private:
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> ciphertext_;
    SchemeType schemeType_;
    types::DataType originalType_;
    size_t stringLength_; // Only used for strings
};

} // namespace hegadb::crypto

#endif // !OPENFHE_CIPHERTEXT_H
