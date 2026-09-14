#ifndef LITERAL_H
#define LITERAL_H

#include "ASTNode.h"
#include "Expr.h"

#include <crypto/Plaintext.h>
#include <string>
#include <utility>
#include <variant>

namespace hegadb {

class Literal : public Expr {
public:
    using Value = PlaintextValue;
    explicit Literal(Value value)
        : value_(std::move(value))
    {
    }
    Value const& value() const { return value_; }
    ASTNodeKind kind() const override { return ASTNodeKind::Literal; }

private:
    Value value_;
};

} // namespace hegadb

#endif
