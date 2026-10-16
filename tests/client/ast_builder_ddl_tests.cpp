#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/CreateTableStmt.h>
#include <hegadb/client/ast/DropTableStmt.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/types/DataType.h>

#include <cassert>
#include <iostream>
#include <memory>

using hegadb::client::SQLParser;
using hegadb::client::ast::ASTNodeKind;
using hegadb::client::ast::CreateTableStmt;
using hegadb::client::ast::DropTableStmt;
using hegadb::client::ast::SelectStmt;
using hegadb::types::DataType;

int main()
{
    SQLParser parser;

    // CREATE TABLE with all four existing types, order preserved
    {
        auto ast = parser.parseToAST(
            "CREATE TABLE users (id INTEGER NOT NULL, name STRING, "
            "active BOOLEAN NOT NULL, amount FLOAT);");

        assert(ast != nullptr);
        assert(ast->kind() == ASTNodeKind::CreateTableStmt);

        auto const* create = dynamic_cast<CreateTableStmt const*>(ast.get());
        assert(create != nullptr);
        assert(create->tableName() == "users");

        auto const& columns = create->columns();
        assert(columns.size() == 4);

        assert(columns[0].name == "id");
        assert(columns[0].type == DataType::Integer);
        assert(!columns[0].nullable);

        assert(columns[1].name == "name");
        assert(columns[1].type == DataType::String);
        assert(columns[1].nullable);

        assert(columns[2].name == "active");
        assert(columns[2].type == DataType::Boolean);
        assert(!columns[2].nullable);

        assert(columns[3].name == "amount");
        assert(columns[3].type == DataType::Float);
        assert(columns[3].nullable);
    }

    // Keywords are case-insensitive, semicolon is optional
    {
        auto ast = parser.parseToAST("create table t (a integer not null)");

        assert(ast != nullptr);

        auto const* create = dynamic_cast<CreateTableStmt const*>(ast.get());
        assert(create != nullptr);
        assert(create->tableName() == "t");
        assert(create->columns().size() == 1);
        assert(create->columns()[0].type == DataType::Integer);
        assert(!create->columns()[0].nullable);
    }

    // DROP TABLE
    {
        auto ast = parser.parseToAST("DROP TABLE users;");

        assert(ast != nullptr);
        assert(ast->kind() == ASTNodeKind::DropTableStmt);

        auto const* drop = dynamic_cast<DropTableStmt const*>(ast.get());
        assert(drop != nullptr);
        assert(drop->tableName() == "users");
    }

    {
        auto ast = parser.parseToAST("drop table orders");

        assert(ast != nullptr);

        auto const* drop = dynamic_cast<DropTableStmt const*>(ast.get());
        assert(drop != nullptr);
        assert(drop->tableName() == "orders");
    }

    // SELECT still works through the same entry point
    {
        auto ast = parser.parseToAST("SELECT * FROM users WHERE id = 1;");

        assert(ast != nullptr);
        assert(ast->kind() == ASTNodeKind::SelectStmt);
        assert(dynamic_cast<SelectStmt const*>(ast.get()) != nullptr);
    }

    // Syntax errors return nullptr
    {
        assert(parser.parseToAST("CREATE TABLE t ()") == nullptr);
        assert(parser.parseToAST("CREATE TABLE t (a BLOB)") == nullptr);
        assert(parser.parseToAST("CREATE TABLE t (a INTEGER,)") == nullptr);
        assert(parser.parseToAST("CREATE TABLE (a INTEGER)") == nullptr);
        assert(parser.parseToAST("CREATE TABLE t (a INTEGER NULL)") == nullptr);
        assert(parser.parseToAST("DROP users") == nullptr);
        assert(parser.parseToAST("DROP TABLE a b") == nullptr);
    }

    std::cout << "AST builder DDL tests passed!\n";

    return 0;
}
