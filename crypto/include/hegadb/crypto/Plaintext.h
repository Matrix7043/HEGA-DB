#ifndef PLAINTEXT_H
#define PLAINTEXT_H

#include <hegadb/types/DataType.h>
#include <hegadb/types/Variant.h>

namespace hegadb::crypto {

using PlaintextValue = types::Variant;

class Plaintext {
public:
    Plaintext(PlaintextValue value);
    types::DataType type() const;
    PlaintextValue const& value() const;

private:
    types::DataType type_;
    PlaintextValue value_;
};

}

#endif // !PLAINTEXT_H
