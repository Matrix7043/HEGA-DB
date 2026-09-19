#ifndef OPENFHE_CONTEXT_H
#define OPENFHE_CONTEXT_H

#include "openfhe.h"

namespace hegadb::crypto {

/**
 * @brief Manages OpenFHE cryptographic contexts for different schemes
 *
 * This class initializes and provides access to BFV and CKKS crypto contexts
 * with appropriate security parameters (128-bit security level).
 */
class OpenFHEContext {
public:
    /**
     * @brief Initialize both BFV and CKKS contexts with security parameters
     */
    OpenFHEContext();

    /**
     * @brief Get the BFV crypto context (for integers, strings, booleans)
     */
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> getBFVContext() const;

    /**
     * @brief Get the CKKS crypto context (for floats)
     */
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> getCKKSContext() const;

    /**
     * @brief Enable features needed for both contexts
     */
    void enableFeatures();

private:
    void initializeBFVContext();
    void initializeCKKSContext();

    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> bfvContext_;
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> ckksContext_;
};

} // namespace hegadb::crypto

#endif 
