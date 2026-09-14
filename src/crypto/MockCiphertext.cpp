#include "crypto/MockCiphertext.h"
#include "crypto/Plaintext.h"
#include <utility>

namespace hegadb {

MockCiphertext::MockCiphertext(
    Plaintext value)
    : value_(std::move(value))
{
}

Plaintext const& MockCiphertext::value() const { return value_; }

}
