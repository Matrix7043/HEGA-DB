#ifndef EXPR_H
#define EXPR_H

#include "ASTNode.h"

namespace hegadb::client::ast {

class Expr : public ASTNode {
public:
    ~Expr() override = default;
};

} // namespace hegadb

#endif
