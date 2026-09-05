#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "SelectStmt.h"
#include <string>

namespace hegadb {

class ASTPrinter {
public:
  void print(const SelectStmt *stmt) const;

private:
  void printExpr(const Expr *expr, const std::string &prefix,
                 bool isLast) const;
};

} // namespace hegadb

#endif
