#ifndef HEGADB_CLIENT_AST_AST_PRINTER_H
#define HEGADB_CLIENT_AST_AST_PRINTER_H

#include <filesystem>
#include <string>

namespace hegadb::client::ast {

class SelectStmt;
class Expr;

class ASTPrinter {
public:
    std::filesystem::path print(
        SelectStmt const* stmt,
        std::filesystem::path const& directory,
        std::string const& queryId) const;

private:
    void print(
        SelectStmt const* stmt,
        std::ostream& output) const;

    void printExpr(
        Expr const* expr,
        std::ostream& output,
        std::string const& prefix,
        bool isLast) const;
};

}

#endif
