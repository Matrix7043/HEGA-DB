#include "hegadb/crypto/KeyManager.h"

namespace hegadb::crypto {

KeyManager::KeyManager(OpenFHEContext const& context)
    : context_(context)
    , keysGenerated_(false)
{
}

void KeyManager::generateKeys()
{
    // Generate BFV key pair
    auto bfvContext = context_.getBFVContext();
    auto bfvKeyPair = bfvContext->KeyGen();
    bfvPublicKey_ = bfvKeyPair.publicKey;
    bfvPrivateKey_ = bfvKeyPair.secretKey;

    // Generate CKKS key pair
    auto ckksContext = context_.getCKKSContext();
    auto ckksKeyPair = ckksContext->KeyGen();
    ckksPublicKey_ = ckksKeyPair.publicKey;
    ckksPrivateKey_ = ckksKeyPair.secretKey;

    keysGenerated_ = true;
}

void KeyManager::generateEvaluationKeys()
{
    if (!keysGenerated_) {
        throw std::runtime_error("Keys must be generated before evaluation keys");
    }

    auto bfvContext = context_.getBFVContext();
    auto ckksContext = context_.getCKKSContext();

    // Generate multiplication/relinearization keys for BFV
    bfvContext->EvalMultKeyGen(bfvPrivateKey_);

    // Generate rotation keys for BFV (needed for string operations)
    // Generate keys for all possible rotations up to half the ring dimension
    std::vector<int32_t> indexList;
    int32_t ringDim = bfvContext->GetRingDimension();
    for (int32_t i = 1; i < static_cast<int32_t>(ringDim / 2); i *= 2) {
        indexList.push_back(i);
        indexList.push_back(-i);
    }
    bfvContext->EvalRotateKeyGen(bfvPrivateKey_, indexList);

    // Generate multiplication/relinearization keys for CKKS
    ckksContext->EvalMultKeyGen(ckksPrivateKey_);

    // Generate rotation keys for CKKS
    ckksContext->EvalRotateKeyGen(ckksPrivateKey_, indexList);
}

lbcrypto::PublicKey<lbcrypto::DCRTPoly> KeyManager::getBFVPublicKey() const
{
    if (!keysGenerated_) {
        throw std::runtime_error("Keys have not been generated yet");
    }
    return bfvPublicKey_;
}

lbcrypto::PrivateKey<lbcrypto::DCRTPoly> KeyManager::getBFVPrivateKey() const
{
    if (!keysGenerated_) {
        throw std::runtime_error("Keys have not been generated yet");
    }
    return bfvPrivateKey_;
}

lbcrypto::PublicKey<lbcrypto::DCRTPoly> KeyManager::getCKKSPublicKey() const
{
    if (!keysGenerated_) {
        throw std::runtime_error("Keys have not been generated yet");
    }
    return ckksPublicKey_;
}

lbcrypto::PrivateKey<lbcrypto::DCRTPoly> KeyManager::getCKKSPrivateKey() const
{
    if (!keysGenerated_) {
        throw std::runtime_error("Keys have not been generated yet");
    }
    return ckksPrivateKey_;
}

bool KeyManager::hasKeys() const
{
    return keysGenerated_;
}

} // namespace hegadb::crypto
