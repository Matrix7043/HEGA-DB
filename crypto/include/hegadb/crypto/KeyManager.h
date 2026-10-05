#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H

#include "OpenFHEContext.h"
#include "openfhe.h"

namespace hegadb::crypto {


class KeyManager {
public:

    explicit KeyManager(OpenFHEContext& context);


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
    
    // Key serialization methods
    std::string serializeBFVPublicKey() const;
    std::string serializeCKKSPublicKey() const;
    
    // Key deserialization methods (for server)
    void deserializeBFVPublicKey(std::string const& serialized);
    void deserializeCKKSPublicKey(std::string const& serialized);
    
    // Key persistence methods
    void saveKeys(std::string const& directory_path) const;
    void loadKeys(std::string const& directory_path);
    bool keyFilesExist(std::string const& directory_path) const;
    
    // Context persistence methods
    void saveContext(std::string const& directory_path) const;
    void loadContext(std::string const& directory_path);

private:
    OpenFHEContext& context_;  // non-owning reference — owned by CompositeEncryption

    // BFV keys
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> bfvPublicKey_;
    lbcrypto::PrivateKey<lbcrypto::DCRTPoly> bfvPrivateKey_;

    // CKKS keys
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> ckksPublicKey_;
    lbcrypto::PrivateKey<lbcrypto::DCRTPoly> ckksPrivateKey_;

    bool keysGenerated_;
    
    // Track individual key availability for deserialization scenarios
    bool bfvPublicKeyAvailable_;
    bool ckksPublicKeyAvailable_;
};

} // namespace hegadb::crypto

#endif // !KEY_MANAGER_H
