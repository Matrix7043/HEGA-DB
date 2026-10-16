#ifndef DROP_TABLE_STMT_H
#define DROP_TABLE_STMT_H

#include "Statement.h"

#include <string>
#include <utility>

namespace hegadb::client::ast {

class DropTableStmt : public Statement {
public:
    explicit DropTableStmt(std::string table_name)
        : table_name_(std::move(table_name))
    {
    }

    std::string const& tableName() const { return table_name_; }

    ASTNodeKind kind() const override { return ASTNodeKind::DropTableStmt; }

private:
    std::string table_name_;
};

} // namespace hegadb::client::ast

#endif // !DROP_TABLE_STMT_H
