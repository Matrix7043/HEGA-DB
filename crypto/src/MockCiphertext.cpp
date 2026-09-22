#include "hegadb/crypto/MockCiphertext.h"
#include "hegadb/crypto/Plaintext.h"
#include <utility>
#include <stdexcept>
#include <string>
#include <variant>


namespace hegadb::crypto {

MockCiphertext::MockCiphertext(
    Plaintext value)
    : value_(std::move(value))
{
}

Plaintext const& MockCiphertext::value() const { return value_; }

std::string MockCiphertext::serialize() const
{
    switch (value_.type()) {
    case types::DataType::Integer:
        return "MOCK:integer:" + std::to_string(std::get<long long>(value_.value()));
 
    case types::DataType::Boolean:
        return std::string("MOCK:boolean:")
            + (std::get<bool>(value_.value()) ? "true" : "false");
 
    case types::DataType::String:
        return "MOCK:string:" + std::get<std::string>(value_.value());
 
    default:
        throw std::runtime_error(
            "MockCiphertext cannot serialize this data type");
    }
}

}
