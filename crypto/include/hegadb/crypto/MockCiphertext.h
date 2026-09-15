#ifndef MOCK_CIPHERTEXT_H
#define MOCK_CIPHERTEXT_H

#include "Ciphertext.h"
#include "Plaintext.h"

namespace hegadb::crypto {

class MockCiphertext final : public Ciphertext {
public:
    explicit MockCiphertext(Plaintext value);
    Plaintext const& value() const;

private:
    Plaintext value_;
};

}

#endif // !MOCK_CIPHERTEXT_H
