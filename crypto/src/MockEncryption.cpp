#include "hegadb/crypto/MockEncryption.h"
#include "hegadb/crypto/Ciphertext.h"
#include "hegadb/crypto/KeyManager.h"
#include "hegadb/crypto/MockCiphertext.h"
#include "hegadb/crypto/Plaintext.h"

#include <memory>
#include <stdexcept>

namespace hegadb::crypto {

std::unique_ptr<Ciphertext> MockEncryption::encrypt(Plaintext const& value)
{
    return std::make_unique<MockCiphertext>(value);
}

std::unique_ptr<Ciphertext> MockEncryption::encryptInsert(Plaintext const& value)
{
    return encrypt(value); // Mock delegates to standard mock ciphertext
}

Plaintext MockEncryption::decrypt(Ciphertext const& value)
{
    auto const* mockCiphertext = dynamic_cast<MockCiphertext const*>(&value);

    if (!mockCiphertext) {
        throw std::runtime_error("MockEncryption received incompatible ciphertext");
    }

    return mockCiphertext->value();
}

KeyManager const& MockEncryption::getKeyManager() const
{
    throw std::runtime_error(
        "MockEncryption::getKeyManager() should not be called. "
        "MockEncryption is a test mock that doesn't manage real keys. "
        "Use CompositeEncryption for production encryption operations.");
}

}
