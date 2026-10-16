#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/ASTParameterizer.h>
#include <hegadb/client/ast/ASTPrinter.h>
#include <hegadb/client/ast/ColumnRef.h>
#include <hegadb/client/ast/JoinClause.h>
#include <hegadb/client/ast/SelectStmt.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using hegadb::client::SQLParser;
using hegadb::client::ast::ASTNodeKind;
using hegadb::client::ast::ColumnRef;
using hegadb::client::ast::SelectStmt;

namespace {

SelectStmt const* asSelect(hegadb::client::ast::Statement const* stmt)
{
    return dynamic_cast<SelectStmt const*>(stmt);
}

} // namespace

int main()
{
    SQLParser parser;

    // INNER JOIN
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM users INNER JOIN orders ON users.id = orders.user_id;");

        assert(ast != nullptr);

        auto const* select = asSelect(ast.get());
        assert(select != nullptr);

        assert(select->from() != nullptr);
        assert(select->from()->name() == "users");
        assert(select->where() == nullptr);

        auto const* join = select->join();
        assert(join != nullptr);
        assert(join->kind() == ASTNodeKind::JoinClause);
        assert(join->table()->name() == "orders");

        assert(join->left()->table().has_value());
        assert(*join->left()->table() == "users");
        assert(join->left()->name() == "id");

        assert(join->right()->table().has_value());
        assert(*join->right()->table() == "orders");
        assert(join->right()->name() == "user_id");

        assert(select->projections().size() == 1);
        assert(select->projections()[0]->kind() == ASTNodeKind::Wildcard);
    }

    // JOIN means INNER JOIN, keywords are case-insensitive,
    // ON columns are kept in the order they were written
    {
        auto ast = parser.parseToAST(
            "select * from users join orders on orders.user_id = users.id");

        assert(ast != nullptr);

        auto const* join = asSelect(ast.get())->join();
        assert(join != nullptr);
        assert(*join->left()->table() == "orders");
        assert(*join->right()->table() == "users");
    }

    // Qualified and unqualified projections
    {
        auto ast = parser.parseToAST(
            "SELECT users.name, amount FROM users JOIN orders "
            "ON users.id = orders.user_id;");

        assert(ast != nullptr);

        auto const* select = asSelect(ast.get());
        assert(select->projections().size() == 2);

        auto const* first = dynamic_cast<ColumnRef const*>(select->projections()[0].get());
        assert(first != nullptr);
        assert(first->table().has_value() && *first->table() == "users");
        assert(first->name() == "name");

        auto const* second = dynamic_cast<ColumnRef const*>(select->projections()[1].get());
        assert(second != nullptr);
        assert(!second->table().has_value());
        assert(second->name() == "amount");
    }

    // A plain SELECT has no join and still parses (qualified column too)
    {
        auto ast = parser.parseToAST("SELECT users.id, name FROM users WHERE id = 1;");

        assert(ast != nullptr);

        auto const* select = asSelect(ast.get());
        assert(select->join() == nullptr);
        assert(select->where() != nullptr);

        auto const* first = dynamic_cast<ColumnRef const*>(select->projections()[0].get());
        assert(*first->table() == "users");
    }

    // Parameterizing a JOIN query keeps the JOIN (it has no literals)
    {
        auto ast = parser.parseToAST(
            "SELECT users.name FROM users JOIN orders ON users.id = orders.user_id;");

        assert(ast != nullptr);

        hegadb::client::ast::ASTParameterizer parameterizer;
        auto parameterized = parameterizer.parameterize(*ast);

        auto const* select = dynamic_cast<SelectStmt const*>(parameterized.get());
        assert(select != nullptr);
        assert(select->from()->name() == "users");

        auto const* join = select->join();
        assert(join != nullptr);
        assert(join->table()->name() == "orders");
        assert(*join->left()->table() == "users");
        assert(join->left()->name() == "id");
        assert(*join->right()->table() == "orders");
        assert(join->right()->name() == "user_id");

        // qualified projections survive too
        auto const* projection = dynamic_cast<ColumnRef const*>(select->projections()[0].get());
        assert(projection != nullptr);
        assert(*projection->table() == "users");
    }

    // The AST printer shows the join
    {
        auto ast = parser.parseToAST(
            "SELECT * FROM users JOIN orders ON users.id = orders.user_id;");
        assert(ast != nullptr);

        auto dir = std::filesystem::temp_directory_path() / "hegadb_join_ast_test";
        hegadb::client::ast::ASTPrinter printer;
        auto path = printer.print(asSelect(ast.get()), dir, "join-1");
        assert(!path.empty());

        std::ifstream in(path);
        std::stringstream text;
        text << in.rdbuf();

        assert(text.str().find("InnerJoin: orders ON users.id = orders.user_id") != std::string::npos);

        std::filesystem::remove_all(dir);
    }

    // Not supported: rejected as syntax errors
    {
        // WHERE together with JOIN
        assert(parser.parseToAST(
                   "SELECT * FROM users JOIN orders ON users.id = orders.user_id "
                   "WHERE users.id = 1")
            == nullptr);
        assert(parser.parseToAST(
                   "SELECT * FROM users WHERE id = 1 "
                   "JOIN orders ON users.id = orders.user_id")
            == nullptr);

        // missing / malformed ON
        assert(parser.parseToAST("SELECT * FROM users JOIN orders") == nullptr);
        assert(parser.parseToAST("SELECT * FROM users JOIN orders ON id = user_id") == nullptr);
        assert(parser.parseToAST("SELECT * FROM users JOIN orders ON users.id = 1") == nullptr);

        // not an equi join / composite condition
        assert(parser.parseToAST(
                   "SELECT * FROM users JOIN orders ON users.id > orders.user_id")
            == nullptr);
        assert(parser.parseToAST(
                   "SELECT * FROM users JOIN orders ON users.id = orders.user_id "
                   "AND users.id = orders.id")
            == nullptr);

        // more than one join
        assert(parser.parseToAST(
                   "SELECT * FROM users JOIN orders ON users.id = orders.user_id "
                   "JOIN items ON orders.id = items.order_id")
            == nullptr);

        // aliases and other join types
        assert(parser.parseToAST(
                   "SELECT * FROM users u JOIN orders o ON u.id = o.user_id")
            == nullptr);
        assert(parser.parseToAST(
                   "SELECT * FROM users LEFT JOIN orders ON users.id = orders.user_id")
            == nullptr);
        assert(parser.parseToAST("SELECT * FROM users, orders") == nullptr);
    }

    std::cout << "AST builder JOIN tests passed!\n";

    return 0;
}
