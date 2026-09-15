#include "hegadb/crypto/MockEncryption.h"
#include "hegadb/crypto/Ciphertext.h"
#include "hegadb/crypto/MockCiphertext.h"
#include "hegadb/crypto/Plaintext.h"

#include <memory>
#include <stdexcept>

namespace hegadb::crypto {

std::unique_ptr<Ciphertext> MockEncryption::encrypt(Plaintext const& value)
{
    return std::make_unique<MockCiphertext>(value);
}

Plaintext MockEncryption::decrypt(Ciphertext const& value)
{
    auto const* mockCiphertext = dynamic_cast<MockCiphertext const*>(&value);

    if (!mockCiphertext) {
        throw std::runtime_error("MockEncryption received incompatible ciphertext");
    }

    return mockCiphertext->value();
}

}
