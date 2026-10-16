#ifndef JOIN_CLAUSE_H
#define JOIN_CLAUSE_H

#include "ASTNode.h"
#include "ColumnRef.h"
#include "TableRef.h"

#include <memory>
#include <utility>

namespace hegadb::client::ast {

// INNER JOIN <table> ON <left> = <right>
//
// Only a single inner equi join on one column pair is supported.
// left() and right() are in the order they were written in the query;
// they are both qualified (table.column).
class JoinClause : public ASTNode {
public:
    JoinClause(std::unique_ptr<TableRef> table,
        std::unique_ptr<ColumnRef> left,
        std::unique_ptr<ColumnRef> right)
        : table_(std::move(table))
        , left_(std::move(left))
        , right_(std::move(right))
    {
    }

    // The joined (right-hand) table
    TableRef const* table() const { return table_.get(); }

    ColumnRef const* left() const { return left_.get(); }
    ColumnRef const* right() const { return right_.get(); }

    ASTNodeKind kind() const override { return ASTNodeKind::JoinClause; }

private:
    std::unique_ptr<TableRef> table_;
    std::unique_ptr<ColumnRef> left_;
    std::unique_ptr<ColumnRef> right_;
};

} // namespace hegadb::client::ast

#endif // !JOIN_CLAUSE_H
