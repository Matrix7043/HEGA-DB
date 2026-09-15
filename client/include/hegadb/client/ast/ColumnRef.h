#ifndef COLUMN_REF_H
#define COLUMN_REF_H

#include "ASTNode.h"
#include "Expr.h"

#include <string>
#include <utility>

namespace hegadb::client::ast {

class ColumnRef : public Expr {
public:
    explicit ColumnRef(std::string name)
        : name_(std::move(name))
    {
    }
    std::string const& name() const { return name_; }
    ASTNodeKind kind() const override { return ASTNodeKind::ColumnRef; }

private:
    std::string name_;
};

} // namespace hegadb

#endif
