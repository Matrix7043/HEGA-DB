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
   // BFV parameters for 128-bit security
    lbcrypto::CCParams<lbcrypto::CryptoContextBFVRNS> parameters;

    // Set security level to 128 bits
    parameters.SetSecurityLevel(lbcrypto::HEStd_128_classic);

    // Ring dimension (meets security requirements)
    parameters.SetRingDim(16384);

    // Plaintext modulus for integer operations
    // Compatible prime that supports SQL INTEGER range (±2 billion)
    // 2013265921 is compatible with ring dimension 16384
    parameters.SetPlaintextModulus(2013265921);  // ← CHANGED

    // Multiplicative depth (number of multiplications supported)
    parameters.SetMultiplicativeDepth(3);

    // Set standard deviation for noise generation
    parameters.SetStandardDeviation(3.2);

    // Create the BFV context
    bfvContext_ = lbcrypto::GenCryptoContext(parameters);
}

void OpenFHEContext::initializeCKKSContext()
{
    // CKKS parameters for 128-bit security
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;

    // Set security level to 128 bits
    parameters.SetSecurityLevel(lbcrypto::HEStd_128_classic);

    // Ring dimension
    parameters.SetRingDim(32768);

    // Multiplicative depth for CKKS
    parameters.SetMultiplicativeDepth(5);

    // Scaling factor bits (controls precision)
    // 50 bits gives good precision for most database float operations
    parameters.SetScalingModSize(50);

    // Batch size (number of values that can be packed into one ciphertext)
    parameters.SetBatchSize(8);

    // Create the CKKS context
    ckksContext_ = lbcrypto::GenCryptoContext(parameters);
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

} // namespace hegadb::crypto
