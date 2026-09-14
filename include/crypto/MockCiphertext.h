#ifndef MOCK_CIPHERTEXT_H
#define MOCK_CIPHERTEXT_H

#include "crypto/Ciphertext.h"
#include "crypto/Plaintext.h"

namespace hegadb {

class MockCiphertext final : public Ciphertext {
public:
    explicit MockCiphertext(Plaintext value);
    Plaintext const& value() const;

private:
    Plaintext value_;
};

}

#endif // !MOCK_CIPHERTEXT_H
