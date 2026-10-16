#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/CreateTableStmt.h>
#include <hegadb/client/ast/DropTableStmt.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/ast/TableRef.h>
#include <hegadb/client/ast/Wildcard.h>
#include <hegadb/client/semantic/SemanticError.h>
#include <hegadb/client/semantic/SemanticValidator.h>

#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/ColumnSchema.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/catalog/db-client/InMemoryCatalog.h>

#include <hegadb/types/DataType.h>

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using namespace hegadb;
using client::SQLParser;
using client::ast::ColumnDef;
using client::ast::CreateTableStmt;
using client::ast::DropTableStmt;
using client::semantic::SemanticError;
using client::semantic::SemanticErrorKind;
using client::semantic::SemanticValidator;
using types::DataType;

namespace {

catalog::InMemoryCatalog createCatalog()
{
    catalog::TableSchema students(
        "STUDENTS",
        { { "NAME", DataType::String, false, 0 },
            { "CLASS", DataType::Integer, false, 1 } });

    std::unordered_map<std::string, catalog::TableSchema> tables;
    tables.emplace("STUDENTS", std::move(students));

    return catalog::InMemoryCatalog(
        std::make_shared<catalog::CatalogSnapshot const>(1, std::move(tables)));
}

// Runs fn and asserts it throws a SemanticError of the given kind.
void expectError(SemanticErrorKind kind, std::function<void()> const& fn)
{
    bool failed = false;

    try {
        fn();
    } catch (SemanticError const& error) {
        failed = true;
        assert(error.kind() == kind);
    }

    assert(failed);
}

CreateTableStmt makeCreate(std::string name, std::vector<ColumnDef> columns)
{
    CreateTableStmt stmt(std::move(name));

    for (auto& column : columns) {
        stmt.addColumn(std::move(column));
    }

    return stmt;
}

} // namespace

int main()
{
    auto catalog = createCatalog();
    SemanticValidator validator(catalog);

    // ---------------- CREATE TABLE ----------------

    // Valid: all four types
    {
        auto stmt = makeCreate("COURSES",
            { { "ID", DataType::Integer, false },
                { "TITLE", DataType::String, true },
                { "CREDITS", DataType::Float, true },
                { "ACTIVE", DataType::Boolean, false } });

        validator.validate(stmt);
    }

    // Table already exists
    {
        auto stmt = makeCreate("STUDENTS",
            { { "ID", DataType::Integer, false } });

        expectError(SemanticErrorKind::TableAlreadyExists,
            [&] { validator.validate(stmt); });
    }

    // Duplicate column name
    {
        auto stmt = makeCreate("COURSES",
            { { "ID", DataType::Integer, false },
                { "ID", DataType::String, true } });

        expectError(SemanticErrorKind::DuplicateColumn,
            [&] { validator.validate(stmt); });
    }

    // No columns
    {
        CreateTableStmt stmt("COURSES");

        expectError(SemanticErrorKind::InvalidTableDefinition,
            [&] { validator.validate(stmt); });
    }

    // Empty table name
    {
        auto stmt = makeCreate("",
            { { "ID", DataType::Integer, false } });

        expectError(SemanticErrorKind::InvalidTableDefinition,
            [&] { validator.validate(stmt); });
    }

    // Empty column name
    {
        auto stmt = makeCreate("COURSES",
            { { "", DataType::Integer, false } });

        expectError(SemanticErrorKind::InvalidTableDefinition,
            [&] { validator.validate(stmt); });
    }

    // ---------------- DROP TABLE ----------------

    {
        DropTableStmt stmt("STUDENTS");
        validator.validate(stmt);
    }

    {
        DropTableStmt stmt("UNKNOWN");

        expectError(SemanticErrorKind::TableNotFound,
            [&] { validator.validate(stmt); });
    }

    // ---------------- Statement dispatcher ----------------

    {
        std::unique_ptr<client::ast::Statement> create
            = std::make_unique<CreateTableStmt>(
                makeCreate("COURSES", { { "ID", DataType::Integer, false } }));

        validator.validate(*create);

        std::unique_ptr<client::ast::Statement> existing
            = std::make_unique<CreateTableStmt>(
                makeCreate("STUDENTS", { { "ID", DataType::Integer, false } }));

        expectError(SemanticErrorKind::TableAlreadyExists,
            [&] { validator.validate(*existing); });

        std::unique_ptr<client::ast::Statement> drop
            = std::make_unique<DropTableStmt>("STUDENTS");

        validator.validate(*drop);

        std::unique_ptr<client::ast::Statement> bad_drop
            = std::make_unique<DropTableStmt>("UNKNOWN");

        expectError(SemanticErrorKind::TableNotFound,
            [&] { validator.validate(*bad_drop); });

        auto select = std::make_unique<client::ast::SelectStmt>();
        select->addProjection(std::make_unique<client::ast::Wildcard>());
        select->setFrom(std::make_unique<client::ast::TableRef>("STUDENTS"));

        std::unique_ptr<client::ast::Statement> select_stmt = std::move(select);

        validator.validate(*select_stmt);
    }

    // ---------------- Parser -> validator ----------------

    {
        SQLParser parser;

        auto create = parser.parseToAST(
            "CREATE TABLE COURSES (ID INTEGER NOT NULL, TITLE STRING);");
        assert(create != nullptr);
        validator.validate(*create);

        auto duplicate = parser.parseToAST(
            "CREATE TABLE COURSES (ID INTEGER, ID STRING);");
        assert(duplicate != nullptr);
        expectError(SemanticErrorKind::DuplicateColumn,
            [&] { validator.validate(*duplicate); });

        auto exists = parser.parseToAST("CREATE TABLE STUDENTS (ID INTEGER);");
        assert(exists != nullptr);
        expectError(SemanticErrorKind::TableAlreadyExists,
            [&] { validator.validate(*exists); });

        auto drop = parser.parseToAST("DROP TABLE STUDENTS;");
        assert(drop != nullptr);
        validator.validate(*drop);

        auto missing = parser.parseToAST("DROP TABLE NOPE;");
        assert(missing != nullptr);
        expectError(SemanticErrorKind::TableNotFound,
            [&] { validator.validate(*missing); });
    }

    std::cout << "Semantic validator DDL tests passed!\n";

    return 0;
}
