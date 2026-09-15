#ifndef WILDCARD_H
#define WILDCARD_H

#include "ASTNode.h"
#include "Expr.h"

namespace hegadb::client::ast {

class Wildcard : public Expr {
public:
    Wildcard() = default;
    ASTNodeKind kind() const override { return ASTNodeKind::Wildcard; }
};

} // namespace hegadb

#endif
