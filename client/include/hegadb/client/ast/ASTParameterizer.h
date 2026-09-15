#ifndef AST_PARAMETERIZER_H
#define AST_PARAMETERIZER_H

#include "ASTNode.h"
#include "Expr.h"

#include <memory>

namespace hegadb::client::ast {

class ASTParameterizer {
public:
    std::unique_ptr<ASTNode> parameterize(ASTNode const& root) const;

private:
    std::unique_ptr<Expr> transform(Expr const& expr, std::size_t& next_parameter_index) const;
};

}

#endif
