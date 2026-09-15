#include "hegadb/crypto/Plaintext.h"

#include <variant>

namespace hegadb::crypto {

Plaintext::Plaintext(PlaintextValue value)
    : value_(std::move(value))
{
    if (std::holds_alternative<long long>(value_)) {
        type_ = types::DataType::Integer;
    } else if (std::holds_alternative<bool>(value_)) {
        type_ = types::DataType::Boolean;
    } else if (std::holds_alternative<std::string>(value_)) {
        type_ = types::DataType::String;
    }
}

types::DataType Plaintext::type() const
{
    return type_;
}

PlaintextValue const& Plaintext::value() const
{
    return value_;
}

}
