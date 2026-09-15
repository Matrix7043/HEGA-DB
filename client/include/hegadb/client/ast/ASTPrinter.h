#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "SelectStmt.h"
#include <string>

namespace hegadb::client::ast {

class ASTPrinter {
public:
    void print(SelectStmt const* stmt) const;

private:
    void printExpr(Expr const* expr, std::string const& prefix,
        bool isLast) const;
};

} // namespace hegadb

#endif
