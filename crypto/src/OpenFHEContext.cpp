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
    // BFV parameters for 128-bit security - EXPLICITLY SET ALL PARAMETERS FOR CONTEXT DETERMINISM
    lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS> parameters;

    // DETERMINISM: Set security level first (affects all other parameters)
    parameters.SetSecurityLevel(lbcrypto::HEStd_128_classic);

    // DETERMINISM: Ring dimension (power of 2, affects all calculations)
    parameters.SetRingDim(16384);

    // DETERMINISM: Plaintext modulus (specific prime)
    parameters.SetPlaintextModulus(2013265921);

    // DETERMINISM: Multiplicative depth (affects modulus chain)
    parameters.SetMultiplicativeDepth(3);

    // DETERMINISM: Noise parameters (must be consistent)
    parameters.SetStandardDeviation(3.2);
    
    // DETERMINISM: Batch size (must be consistent)
    parameters.SetBatchSize(8192);
    
    // DETERMINISM: Key distribution (affects key generation algorithm)
    parameters.SetSecretKeyDist(lbcrypto::UNIFORM_TERNARY);
    
    // DETERMINISM: Key switching technique (affects context structure)
    parameters.SetKeySwitchTechnique(lbcrypto::HYBRID);
    
    // Create context with fully specified parameters
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
