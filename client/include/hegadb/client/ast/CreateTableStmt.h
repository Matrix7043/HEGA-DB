#ifndef CREATE_TABLE_STMT_H
#define CREATE_TABLE_STMT_H

#include "Statement.h"

#include <hegadb/types/DataType.h>

#include <string>
#include <utility>
#include <vector>

namespace hegadb::client::ast {

// One column in a CREATE TABLE statement.
// The ordinal is not stored here: it is the column's position in
// CreateTableStmt::columns().
struct ColumnDef {
    std::string name;
    types::DataType type;
    bool nullable;
};

class CreateTableStmt : public Statement {
public:
    explicit CreateTableStmt(std::string table_name)
        : table_name_(std::move(table_name))
    {
    }

    void addColumn(ColumnDef column) { columns_.push_back(std::move(column)); }

    std::string const& tableName() const { return table_name_; }

    std::vector<ColumnDef> const& columns() const { return columns_; }

    ASTNodeKind kind() const override { return ASTNodeKind::CreateTableStmt; }

private:
    std::string table_name_;
    std::vector<ColumnDef> columns_;
};

} // namespace hegadb::client::ast

#endif // !CREATE_TABLE_STMT_H
