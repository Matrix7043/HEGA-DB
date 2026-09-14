#ifndef PLAINTEXT_H
#define PLAINTEXT_H

#include <catalog/DataType.h>

#include <string>
#include <variant>

namespace hegadb {

using PlaintextValue = std::variant<long long, bool, std::string>;

class Plaintext {
public:
    Plaintext(PlaintextValue value);
    DataType type() const;
    PlaintextValue const& value() const;

private:
    DataType type_;
    PlaintextValue value_;
};

}

#endif // !PLAINTEXT_H
