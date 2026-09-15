#ifndef STATEMENT_H
#define STATEMENT_H

#include "ASTNode.h"

namespace hegadb::client::ast {

class Statement : public ASTNode {
public:
    ~Statement() override = default;
};

} // namespace hegadb

#endif
