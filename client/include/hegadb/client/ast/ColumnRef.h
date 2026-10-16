#ifndef COLUMN_REF_H
#define COLUMN_REF_H

#include "ASTNode.h"
#include "Expr.h"

#include <optional>
#include <string>
#include <utility>

namespace hegadb::client::ast {

class ColumnRef : public Expr {
public:
    // Unqualified column: name
    explicit ColumnRef(std::string name)
        : name_(std::move(name))
    {
    }

    // Qualified column: table.name
    ColumnRef(std::string table, std::string name)
        : table_(std::move(table))
        , name_(std::move(name))
    {
    }

    std::string const& name() const { return name_; }

    // The table qualifier, if the column was written as table.name
    std::optional<std::string> const& table() const { return table_; }

    ASTNodeKind kind() const override { return ASTNodeKind::ColumnRef; }

private:
    std::optional<std::string> table_;
    std::string name_;
};

} // namespace hegadb

#endif
