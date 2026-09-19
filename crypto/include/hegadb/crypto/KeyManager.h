#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H

#include "OpenFHEContext.h"
#include "openfhe.h"

namespace hegadb::crypto {


class KeyManager {
public:

    explicit KeyManager(OpenFHEContext const& context);


    void generateKeys();

    /**
     * This includes:
     * - Multiplication/relinearization keys
     * - Rotation keys (for SIMD operations)
     */
    void generateEvaluationKeys();

    // BFV key accessors
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> getBFVPublicKey() const;
    lbcrypto::PrivateKey<lbcrypto::DCRTPoly> getBFVPrivateKey() const;

    // CKKS key accessors
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> getCKKSPublicKey() const;
    lbcrypto::PrivateKey<lbcrypto::DCRTPoly> getCKKSPrivateKey() const;


    bool hasKeys() const;

private:
    OpenFHEContext context_;

    // BFV keys
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> bfvPublicKey_;
    lbcrypto::PrivateKey<lbcrypto::DCRTPoly> bfvPrivateKey_;

    // CKKS keys
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> ckksPublicKey_;
    lbcrypto::PrivateKey<lbcrypto::DCRTPoly> ckksPrivateKey_;

    bool keysGenerated_;
};

} // namespace hegadb::crypto

#endif // !KEY_MANAGER_H
