#ifndef COMPOSITE_ENCRYPTION_H
#define COMPOSITE_ENCRYPTION_H

#include "BFVEncryption.h"
#include "CKKSEncryption.h"
#include "EncryptionWrapper.h"
#include "KeyManager.h"
#include "OpenFHEContext.h"

#include <memory>

namespace hegadb::crypto {


class CompositeEncryption final : public EncryptionWrapper {
public:
    
    CompositeEncryption();

    std::unique_ptr<Ciphertext> encrypt(Plaintext const& value) override;

    Plaintext decrypt(Ciphertext const& value) override;

    OpenFHEContext const& getContext() const;

    KeyManager const& getKeyManager() const;

private:
    OpenFHEContext context_;
    KeyManager keyManager_;
    std::unique_ptr<BFVEncryption> bfvEngine_;
    std::unique_ptr<CKKSEncryption> ckksEngine_;
};

} // namespace hegadb::crypto

#endif // !COMPOSITE_ENCRYPTION_H
