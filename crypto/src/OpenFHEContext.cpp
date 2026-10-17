#include "hegadb/crypto/OpenFHEContext.h"

namespace hegadb::crypto {

OpenFHEContext::OpenFHEContext()
{
    initializeBFVContext();
    initializeCKKSContext();
    enableFeatures();
}

void OpenFHEContext::initializeBFVContext()
{
    // BFV parameters configured for 8-bit signed integer columns [-128, 127].
    //
    // Design rationale:
    //   * PlaintextModulus p = 65537 = 2^16 + 1 (Fermat/NTT prime).
    //     All signed 8-bit values fit in Z_65537.  Negative values are stored
    //     as their p-complement: e.g. -1 → 65536, -128 → 65409.
    //
    //   * The Fermat-residue equality indicator computes diff^(p-1) = diff^65536.
    //     65536 = 2^16 → exactly 16 squarings → MultiplicativeDepth = 16.
    //
    //   * For LT/GT/LE/GE the evaluator sums EQ-indicators over the negative
    //     residue interval [(p+1)/2, p-1] = [32769, 65536].  With 8-bit signed
    //     arithmetic the maximum |diff| is 255, so only 255 of those candidates
    //     (the range [65282, 65536]) are ever non-zero, keeping the loop bounded.
    //
    //   * RingDim is left to OpenFHE to choose automatically based on
    //     HEStd_128_classic + depth 16 (typically selects 32768 for 128-bit
    //     security).  Do NOT pin it here to avoid silently weakening security.
    lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS> parameters;

    // Security level drives automatic RingDim selection
    parameters.SetSecurityLevel(lbcrypto::HEStd_128_classic);

    // Plaintext modulus: 65537 = 2^16 + 1 (NTT-friendly Fermat prime)
    parameters.SetPlaintextModulus(65537);

    // Multiplicative depth: 16 squarings needed for diff^(p-1) = diff^65536
    parameters.SetMultiplicativeDepth(16);

    // Noise parameters
    parameters.SetStandardDeviation(3.2);

    // Key generation settings
    parameters.SetSecretKeyDist(lbcrypto::UNIFORM_TERNARY);
    parameters.SetKeySwitchTechnique(lbcrypto::HYBRID);

    bfvContext_ = lbcrypto::GenCryptoContext(parameters);

    if (!bfvContext_) {
        throw std::runtime_error("Failed to create BFV context");
    }
}

void OpenFHEContext::initializeCKKSContext()
{
    // CKKS parameters for 128-bit security - EXPLICITLY SET ALL PARAMETERS FOR CONTEXT DETERMINISM
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;

    // DETERMINISM: Set security level first (affects all other parameters)
    parameters.SetSecurityLevel(lbcrypto::HEStd_128_classic);

    // DETERMINISM: Ring dimension (power of 2)
    parameters.SetRingDim(32768);

    // DETERMINISM: Multiplicative depth (affects modulus chain)
    parameters.SetMultiplicativeDepth(5);

    // DETERMINISM: Scaling parameters (critical for CKKS determinism)
    parameters.SetScalingModSize(50);
    parameters.SetFirstModSize(60);

    // DETERMINISM: Batch size (must be consistent)
    parameters.SetBatchSize(16384);
    
    // DETERMINISM: Key distribution (affects key generation algorithm)
    parameters.SetSecretKeyDist(lbcrypto::UNIFORM_TERNARY);
    
    // DETERMINISM: Noise parameters (must be consistent)
    parameters.SetStandardDeviation(3.2);
    
    // DETERMINISM: Key switching technique (affects context structure)
    parameters.SetKeySwitchTechnique(lbcrypto::HYBRID);

    // Create context with fully specified parameters
    ckksContext_ = lbcrypto::GenCryptoContext(parameters);
    
    if (!ckksContext_) {
        throw std::runtime_error("Failed to create CKKS context");
    }
}

void OpenFHEContext::enableFeatures()
{
    // Enable features for BFV context
    bfvContext_->Enable(lbcrypto::PKE);           // Public Key Encryption
    bfvContext_->Enable(lbcrypto::KEYSWITCH);     // Key Switching
    bfvContext_->Enable(lbcrypto::LEVELEDSHE);    // Leveled SHE operations
    bfvContext_->Enable(lbcrypto::ADVANCEDSHE);   // Advanced operations

    // Enable features for CKKS context
    ckksContext_->Enable(lbcrypto::PKE);
    ckksContext_->Enable(lbcrypto::KEYSWITCH);
    ckksContext_->Enable(lbcrypto::LEVELEDSHE);
    ckksContext_->Enable(lbcrypto::ADVANCEDSHE);
}

lbcrypto::CryptoContext<lbcrypto::DCRTPoly> OpenFHEContext::getBFVContext() const
{
    return bfvContext_;
}

lbcrypto::CryptoContext<lbcrypto::DCRTPoly> OpenFHEContext::getCKKSContext() const
{
    return ckksContext_;
}

void OpenFHEContext::setBFVContext(lbcrypto::CryptoContext<lbcrypto::DCRTPoly> context)
{
    bfvContext_ = context;
}

void OpenFHEContext::setCKKSContext(lbcrypto::CryptoContext<lbcrypto::DCRTPoly> context)
{
    ckksContext_ = context;
}

} // namespace hegadb::crypto
