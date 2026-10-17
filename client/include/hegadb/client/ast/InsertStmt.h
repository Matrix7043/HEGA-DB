#ifndef INSERT_STMT_H
#define INSERT_STMT_H

#include "Expr.h"
#include "Statement.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace hegadb::client::ast {

// INSERT INTO <table> [(<column>, ...)] VALUES (<value>, ...) [, (...)]*
//
// Every value is an Expr: a Literal or a NullLiteral when freshly parsed.
// Query preparation later replaces each Literal with a ParameterRef, the same
// way it does for literals in a WHERE clause.
class InsertStmt : public Statement {
public:
    using Row = std::vector<std::unique_ptr<Expr>>;

    explicit InsertStmt(std::string table_name)
        : table_name_(std::move(table_name))
    {
    }

    void addColumn(std::string column) { columns_.push_back(std::move(column)); }

    void addRow(Row row) { rows_.push_back(std::move(row)); }

    std::string const& tableName() const { return table_name_; }

    // The column list as written. Empty when the statement has none, in which
    // case each row provides a value for every column of the table.
    std::vector<std::string> const& columns() const { return columns_; }

    bool hasColumnList() const { return !columns_.empty(); }

    std::vector<Row> const& rows() const { return rows_; }

    ASTNodeKind kind() const override { return ASTNodeKind::InsertStmt; }

private:
    std::string table_name_;
    std::vector<std::string> columns_;
    std::vector<Row> rows_;
};

} // namespace hegadb::client::ast

#endif // !INSERT_STMT_H
