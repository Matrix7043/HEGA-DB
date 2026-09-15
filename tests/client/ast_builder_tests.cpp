#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/ASTBuilder.h>
#include <hegadb/client/ast/ColumnRef.h>
#include <hegadb/client/ast/SelectStmt.h>

#include <HEGADBLexer.h>
#include <HEGADBParser.h>

#include <antlr4-runtime.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

int main()
{
    std::string sql = "SELECT salary FROM employees;";

    antlr4::ANTLRInputStream input(sql);

    HEGADBLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);

    HEGADBParser parser(&tokens);

    auto* tree = parser.query();

    assert(tree != nullptr);
    assert(parser.getNumberOfSyntaxErrors() == 0);

    hegadb::client::ast::ASTBuilder builder;

    auto result = builder.visit(tree);

    auto* stmt = std::any_cast<hegadb::client::ast::SelectStmt*>(result);

    assert(stmt != nullptr);

    std::unique_ptr<hegadb::client::ast::SelectStmt> query(stmt);

    assert(
        query->kind() == hegadb::client::ast::ASTNodeKind::SelectStmt);

    assert(query->projections().size() == 1);

    assert(query->from() != nullptr);
    assert(query->from()->name() == "employees");

    auto* column = dynamic_cast<hegadb::client::ast::ColumnRef const*>(
        query->projections()[0].get());

    assert(column != nullptr);
    assert(column->name() == "salary");

    std::cout << "ASTBuilder test passed!\n";

    return 0;
}
