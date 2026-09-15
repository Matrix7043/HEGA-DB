#ifndef LITERAL_H
#define LITERAL_H

#include "ASTNode.h"
#include "Expr.h"
#include "hegadb/crypto/Plaintext.h"

#include <utility>

namespace hegadb::client::ast {

class Literal : public Expr {
public:
    using Value = crypto::PlaintextValue;
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
