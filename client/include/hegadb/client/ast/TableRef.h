#ifndef TABLE_REF_H
#define TABLE_REF_H

#include "ASTNode.h"

#include <string>
#include <utility>

namespace hegadb::client::ast {

class TableRef : public ASTNode {
public:
    explicit TableRef(std::string name)
        : name_(std::move(name))
    {
    }
    std::string const& name() const { return name_; }
    ASTNodeKind kind() const override { return ASTNodeKind::TableRef; }

private:
    std::string name_;
};

} // namespace hegadb

#endif
