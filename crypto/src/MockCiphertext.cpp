#include "hegadb/crypto/MockCiphertext.h"
#include "hegadb/crypto/Plaintext.h"
#include <utility>

namespace hegadb::crypto {

MockCiphertext::MockCiphertext(
    Plaintext value)
    : value_(std::move(value))
{
}

Plaintext const& MockCiphertext::value() const { return value_; }

}
