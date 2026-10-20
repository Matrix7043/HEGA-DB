#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/CreateTableStmt.h>
#include <hegadb/client/ast/DropTableStmt.h>
#include <hegadb/client/ast/InsertStmt.h>
#include <hegadb/client/ast/Literal.h>
#include <hegadb/client/ast/NullLiteral.h>
#include <hegadb/client/ast/SelectStmt.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <variant>

using hegadb::client::SQLParser;
using hegadb::client::ast::ASTNodeKind;
using hegadb::client::ast::Expr;
using hegadb::client::ast::InsertStmt;
using hegadb::client::ast::Literal;
using hegadb::client::ast::NullLiteral;
using hegadb::client::ast::SelectStmt;

namespace {

InsertStmt const& asInsert(std::unique_ptr<hegadb::client::ast::Statement> const& ast)
{
    assert(ast != nullptr);
    assert(ast->kind() == ASTNodeKind::InsertStmt);

    auto const* insert = dynamic_cast<InsertStmt const*>(ast.get());
    assert(insert != nullptr);

    return *insert;
}

// Returns the cell as a Literal, asserting that it is one.
Literal const& literalAt(InsertStmt const& insert, std::size_t row, std::size_t column)
{
    Expr const* cell = insert.rows().at(row).at(column).get();
    assert(cell != nullptr);
    assert(cell->kind() == ASTNodeKind::Literal);

    return static_cast<Literal const&>(*cell);
}

bool isNull(InsertStmt const& insert, std::size_t row, std::size_t column)
{
    Expr const* cell = insert.rows().at(row).at(column).get();
    assert(cell != nullptr);

    return cell->kind() == ASTNodeKind::NullLiteral
        && dynamic_cast<NullLiteral const*>(cell) != nullptr;
}

} // namespace

int main()
{
    SQLParser parser;

    // INSERT without a column list
    {
        auto ast = parser.parseToAST("INSERT INTO users VALUES (1, 'alice');");
        auto const& insert = asInsert(ast);

        assert(insert.tableName() == "users");
        assert(!insert.hasColumnList());
        assert(insert.columns().empty());

        assert(insert.rows().size() == 1);
        assert(insert.rows()[0].size() == 2);

        assert(std::get<long long>(literalAt(insert, 0, 0).value()) == 1);
        assert(std::get<std::string>(literalAt(insert, 0, 1).value()) == "alice");
    }

    // INSERT with a column list: the order is kept
    {
        auto ast = parser.parseToAST(
            "INSERT INTO users (name, id) VALUES ('alice', 1);");
        auto const& insert = asInsert(ast);

        assert(insert.tableName() == "users");
        assert(insert.hasColumnList());

        assert(insert.columns().size() == 2);
        assert(insert.columns()[0] == "name");
        assert(insert.columns()[1] == "id");

        assert(insert.rows().size() == 1);
        assert(std::get<std::string>(literalAt(insert, 0, 0).value()) == "alice");
        assert(std::get<long long>(literalAt(insert, 0, 1).value()) == 1);
    }

    // Several rows, in order
    {
        auto ast = parser.parseToAST(
            "INSERT INTO users (id, name) VALUES "
            "(1, 'alice'), (2, 'bob'), (3, 'carol');");
        auto const& insert = asInsert(ast);

        assert(insert.rows().size() == 3);

        for (std::size_t row = 0; row < 3; ++row) {
            assert(insert.rows()[row].size() == 2);
            assert(std::get<long long>(literalAt(insert, row, 0).value())
                == static_cast<long long>(row + 1));
        }

        assert(std::get<std::string>(literalAt(insert, 0, 1).value()) == "alice");
        assert(std::get<std::string>(literalAt(insert, 1, 1).value()) == "bob");
        assert(std::get<std::string>(literalAt(insert, 2, 1).value()) == "carol");
    }

    // Every kind of value
    {
        auto ast = parser.parseToAST(
            "INSERT INTO t VALUES (42, 'text', TRUE, FALSE, NULL);");
        auto const& insert = asInsert(ast);

        assert(insert.rows().size() == 1);
        assert(insert.rows()[0].size() == 5);

        assert(std::get<long long>(literalAt(insert, 0, 0).value()) == 42);
        assert(std::get<std::string>(literalAt(insert, 0, 1).value()) == "text");
        assert(std::get<bool>(literalAt(insert, 0, 2).value()) == true);
        assert(std::get<bool>(literalAt(insert, 0, 3).value()) == false);
        assert(isNull(insert, 0, 4));
    }

    // NULL is a NullLiteral node, not a Literal
    {
        auto ast = parser.parseToAST("INSERT INTO t (a, b) VALUES (NULL, 1);");
        auto const& insert = asInsert(ast);

        assert(isNull(insert, 0, 0));
        assert(!isNull(insert, 0, 1));
        assert(insert.rows()[0][0]->kind() == ASTNodeKind::NullLiteral);
        assert(insert.rows()[0][1]->kind() == ASTNodeKind::Literal);
    }

    // The quotes around a string are removed
    {
        auto ast = parser.parseToAST("INSERT INTO t VALUES ('hello world', '');");
        auto const& insert = asInsert(ast);

        assert(std::get<std::string>(literalAt(insert, 0, 0).value()) == "hello world");
        assert(std::get<std::string>(literalAt(insert, 0, 1).value()).empty());
    }

    // The statement is case-insensitive and the semicolon is optional
    {
        auto ast = parser.parseToAST("insert into Users (Id) values (7)");
        auto const& insert = asInsert(ast);

        // Identifiers keep their case.
        assert(insert.tableName() == "Users");
        assert(insert.columns().size() == 1);
        assert(insert.columns()[0] == "Id");
        assert(std::get<long long>(literalAt(insert, 0, 0).value()) == 7);
    }

    // A row can have a different shape to the others. This is a
    // semantic error, so the parser accepts it.
    {
        auto ast = parser.parseToAST(
            "INSERT INTO t VALUES (1, 2), (3);");
        auto const& insert = asInsert(ast);

        assert(insert.rows().size() == 2);
        assert(insert.rows()[0].size() == 2);
        assert(insert.rows()[1].size() == 1);
    }

    // The same column listed twice is also a semantic error
    {
        auto ast = parser.parseToAST("INSERT INTO t (a, a) VALUES (1, 2);");
        auto const& insert = asInsert(ast);

        assert(insert.columns().size() == 2);
        assert(insert.columns()[0] == "a");
        assert(insert.columns()[1] == "a");
    }

    // The node reports its own kind and is a Statement
    {
        InsertStmt insert("t");
        assert(insert.kind() == ASTNodeKind::InsertStmt);
        assert(insert.tableName() == "t");
        assert(!insert.hasColumnList());
        assert(insert.rows().empty());

        NullLiteral null_value;
        assert(null_value.kind() == ASTNodeKind::NullLiteral);

        hegadb::client::ast::Statement const& as_statement = insert;
        assert(as_statement.kind() == ASTNodeKind::InsertStmt);
    }

    // Syntax errors return nullptr
    {
        assert(parser.parseToAST("INSERT INTO t VALUES ()") == nullptr);
        assert(parser.parseToAST("INSERT INTO t VALUES (1,)") == nullptr);
        assert(parser.parseToAST("INSERT t VALUES (1)") == nullptr);
        assert(parser.parseToAST("INSERT INTO t (a) (1)") == nullptr);
        assert(parser.parseToAST("INSERT INTO t () VALUES (1)") == nullptr);
        assert(parser.parseToAST("INSERT INTO t VALUES (a)") == nullptr);
    }

    // The other statements still build the same nodes
    {
        auto select = parser.parseToAST("SELECT * FROM users WHERE id = 1;");
        assert(select != nullptr);
        assert(select->kind() == ASTNodeKind::SelectStmt);
        assert(dynamic_cast<SelectStmt const*>(select.get()) != nullptr);

        auto create = parser.parseToAST("CREATE TABLE t (a INTEGER);");
        assert(create != nullptr);
        assert(create->kind() == ASTNodeKind::CreateTableStmt);

        auto drop = parser.parseToAST("DROP TABLE t;");
        assert(drop != nullptr);
        assert(drop->kind() == ASTNodeKind::DropTableStmt);
    }

    std::cout << "AST builder INSERT tests passed!\n";

    return 0;
}
