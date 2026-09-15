#ifndef PARAMETER_REF_H
#define PARAMETER_REF_H

#include "ASTNode.h"
#include "Expr.h"

#include <cstddef>

namespace hegadb::client::ast {

class ParameterRef : public Expr {

public:
    explicit ParameterRef(std::size_t parameter_index)
        : parameter_index_(parameter_index)
    {
    }

    std::size_t parameterIndex() const { return parameter_index_; }

    ASTNodeKind kind() const override { return ASTNodeKind::ParameterRef; }

private:
    std::size_t parameter_index_;
};

}

#endif // !PARAMETER_REF_H
