#ifndef PLAINTEXT_H
#define PLAINTEXT_H

#include <catalog/DataType.h>

#include <cstdint>
#include <string>
#include <variant>

namespace hegadb {

using PlaintextValue = std::variant<std::uint64_t, bool, std::string, double>;

class Plaintext {
public:
    Plaintext(DataType type, PlaintextValue value);
    DataType type() const;
    PlaintextValue const& value() const;

private:
    DataType type_;
    PlaintextValue value_;
};

}

#endif // !PLAINTEXT_H
