#ifndef SELECT_STMT_H
#define SELECT_STMT_H

#include "Expr.h"
#include "Statement.h"
#include "TableRef.h"

#include <memory>
#include <vector>

namespace hegadb::client::ast {

class SelectStmt : public Statement {
public:
    void addProjection(std::unique_ptr<Expr> column)
    {
        projections_.push_back(std::move(column));
    }
    void setFrom(std::unique_ptr<TableRef> table) { from_ = std::move(table); }
    std::vector<std::unique_ptr<Expr>> const& projections() const
    {
        return projections_;
    }
    TableRef const* from() const { return from_.get(); }

    void setWhere(std::unique_ptr<Expr> expr) { where_ = std::move(expr); }

    Expr const* where() const { return where_.get(); }

    ASTNodeKind kind() const override { return ASTNodeKind::SelectStmt; }

private:
    std::vector<std::unique_ptr<Expr>> projections_;
    std::unique_ptr<TableRef> from_;
    std::unique_ptr<Expr> where_;
};

} // namespace hegadb

#endif
