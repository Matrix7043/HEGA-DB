#include "crypto/Plaintext.h"

#include <variant>

namespace hegadb {

Plaintext::Plaintext(PlaintextValue value)
    : value_(std::move(value))
{
    if (std::holds_alternative<long long>(value_)) {
        type_ = DataType::Integer;
    } else if (std::holds_alternative<bool>(value_)) {
        type_ = DataType::Boolean;
    } else if (std::holds_alternative<std::string>(value_)) {
        type_ = DataType::String;
    }
}

DataType Plaintext::type() const
{
    return type_;
}

PlaintextValue const& Plaintext::value() const
{
    return value_;
}

}
