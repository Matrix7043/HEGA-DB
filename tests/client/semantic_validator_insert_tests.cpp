#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/InsertStmt.h>
#include <hegadb/client/semantic/SemanticError.h>
#include <hegadb/client/semantic/SemanticValidator.h>

#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/ColumnSchema.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/catalog/db-client/InMemoryCatalog.h>

#include <hegadb/types/DataType.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

using namespace hegadb;
using client::SQLParser;
using client::semantic::SemanticError;
using client::semantic::SemanticErrorKind;
using client::semantic::SemanticValidator;
using types::DataType;

namespace {

catalog::InMemoryCatalog createCatalog()
{
    std::unordered_map<std::string, catalog::TableSchema> tables;

    // ID and ACTIVE are NOT NULL; NAME and SCORE are nullable.
    tables.emplace("STUDENTS",
        catalog::TableSchema("STUDENTS",
            { { "ID", DataType::Integer, false, 0 },
                { "NAME", DataType::String, true, 1 },
                { "ACTIVE", DataType::Boolean, false, 2 },
                { "SCORE", DataType::Float, true, 3 } }));

    // Columns stored in a different order to their ordinals: values
    // without a column list follow the ordinals.
    tables.emplace("SHUFFLED",
        catalog::TableSchema("SHUFFLED",
            { { "B", DataType::String, false, 1 },
                { "A", DataType::Integer, false, 0 } }));

    return catalog::InMemoryCatalog(
        std::make_shared<catalog::CatalogSnapshot const>(1, std::move(tables)));
}

SQLParser parser;

// Parses sql, which must be an INSERT, and validates it.
void validate(SemanticValidator const& validator, std::string const& sql)
{
    auto ast = parser.parseToAST(sql);
    assert(ast != nullptr);
    validator.validate(*ast); // dispatches on the statement kind
}

void expectValid(SemanticValidator const& validator, std::string const& sql)
{
    try {
        validate(validator, sql);
    } catch (SemanticError const& error) {
        std::cerr << "unexpected error for: " << sql << "\n  " << error.what() << '\n';
        assert(false);
    }
}

void expectError(SemanticValidator const& validator, SemanticErrorKind kind,
    std::string const& sql)
{
    try {
        validate(validator, sql);
    } catch (SemanticError const& error) {
        if (error.kind() != kind) {
            std::cerr << "wrong error kind for: " << sql << "\n  " << error.what() << '\n';
        }
        assert(error.kind() == kind);
        return;
    }

    std::cerr << "expected an error for: " << sql << '\n';
    assert(false);
}

} // namespace

int main()
{
    auto catalog = createCatalog();
    SemanticValidator validator(catalog);

    // ---------------- Valid ----------------

    // All columns, no column list
    expectValid(validator, "INSERT INTO STUDENTS VALUES (1, 'alice', TRUE, NULL);");

    // NULL in nullable columns
    expectValid(validator, "INSERT INTO STUDENTS VALUES (1, NULL, TRUE, NULL);");

    // Column list in a different order to the table
    expectValid(validator, "INSERT INTO STUDENTS (ACTIVE, ID, NAME) VALUES (FALSE, 1, 'a');");

    // Columns left out are nullable ones
    expectValid(validator, "INSERT INTO STUDENTS (ID, ACTIVE) VALUES (1, TRUE);");

    // Several rows
    expectValid(validator,
        "INSERT INTO STUDENTS (ID, ACTIVE) VALUES (1, TRUE), (2, FALSE), (3, TRUE);");

    // Without a column list, values follow the column ordinals
    expectValid(validator, "INSERT INTO SHUFFLED VALUES (1, 'x');");

    // The statement can be validated through the dedicated overload too
    {
        auto ast = parser.parseToAST("INSERT INTO STUDENTS (ID, ACTIVE) VALUES (1, TRUE);");
        assert(ast != nullptr);
        validator.validate(static_cast<client::ast::InsertStmt const&>(*ast));
    }

    // ---------------- Table and columns ----------------

    expectError(validator, SemanticErrorKind::TableNotFound,
        "INSERT INTO MISSING VALUES (1);");

    expectError(validator, SemanticErrorKind::ColumnNotFound,
        "INSERT INTO STUDENTS (ID, NOPE) VALUES (1, 2);");

    expectError(validator, SemanticErrorKind::DuplicateColumn,
        "INSERT INTO STUDENTS (ID, ID, ACTIVE) VALUES (1, 2, TRUE);");

    // Identifiers are case-sensitive, like in SELECT
    expectError(validator, SemanticErrorKind::TableNotFound,
        "INSERT INTO students VALUES (1, 'a', TRUE, NULL);");

    // ---------------- Value counts ----------------

    // Too few values without a column list
    expectError(validator, SemanticErrorKind::ValueCountMismatch,
        "INSERT INTO STUDENTS VALUES (1, 'alice');");

    // Too many values without a column list
    expectError(validator, SemanticErrorKind::ValueCountMismatch,
        "INSERT INTO STUDENTS VALUES (1, 'alice', TRUE, NULL, 5);");

    // Too few / many values with a column list
    expectError(validator, SemanticErrorKind::ValueCountMismatch,
        "INSERT INTO STUDENTS (ID, ACTIVE) VALUES (1);");
    expectError(validator, SemanticErrorKind::ValueCountMismatch,
        "INSERT INTO STUDENTS (ID, ACTIVE) VALUES (1, TRUE, 'x');");

    // Only one row is wrong: it is still rejected
    expectError(validator, SemanticErrorKind::ValueCountMismatch,
        "INSERT INTO STUDENTS (ID, ACTIVE) VALUES (1, TRUE), (2);");

    // ---------------- Types ----------------

    expectError(validator, SemanticErrorKind::TypeMismatch,
        "INSERT INTO STUDENTS (ID, ACTIVE) VALUES ('one', TRUE);");

    expectError(validator, SemanticErrorKind::TypeMismatch,
        "INSERT INTO STUDENTS (ID, ACTIVE) VALUES (1, 1);");

    expectError(validator, SemanticErrorKind::TypeMismatch,
        "INSERT INTO STUDENTS (ID, NAME, ACTIVE) VALUES (1, 5, TRUE);");

    expectError(validator, SemanticErrorKind::TypeMismatch,
        "INSERT INTO STUDENTS (ID, NAME, ACTIVE) VALUES (1, TRUE, TRUE);");

    // A bad value in a later row is found
    expectError(validator, SemanticErrorKind::TypeMismatch,
        "INSERT INTO STUDENTS (ID, ACTIVE) VALUES (1, TRUE), (2, 'no');");

    // Values follow the ordinals, so SHUFFLED's first value is A (INTEGER)
    expectError(validator, SemanticErrorKind::TypeMismatch,
        "INSERT INTO SHUFFLED VALUES ('x', 1);");

    // FLOAT columns take only NULL: there are no float literals
    expectError(validator, SemanticErrorKind::TypeMismatch,
        "INSERT INTO STUDENTS VALUES (1, 'a', TRUE, 5);");
    expectValid(validator, "INSERT INTO STUDENTS VALUES (1, 'a', TRUE, NULL);");

    // ---------------- NULL ----------------

    // NULL in a NOT NULL column
    expectError(validator, SemanticErrorKind::NullConstraintViolation,
        "INSERT INTO STUDENTS VALUES (NULL, 'a', TRUE, NULL);");

    expectError(validator, SemanticErrorKind::NullConstraintViolation,
        "INSERT INTO STUDENTS (ID, ACTIVE) VALUES (1, NULL);");

    // NULL in a later row
    expectError(validator, SemanticErrorKind::NullConstraintViolation,
        "INSERT INTO STUDENTS (ID, ACTIVE) VALUES (1, TRUE), (NULL, TRUE);");

    // A NOT NULL column that is left out of the column list
    expectError(validator, SemanticErrorKind::NullConstraintViolation,
        "INSERT INTO STUDENTS (ID, NAME) VALUES (1, 'a');");

    expectError(validator, SemanticErrorKind::NullConstraintViolation,
        "INSERT INTO STUDENTS (NAME) VALUES ('a');");

    // ---------------- Other statements are unaffected ----------------

    {
        auto select = parser.parseToAST("SELECT * FROM STUDENTS;");
        assert(select != nullptr);
        validator.validate(*select);

        auto drop = parser.parseToAST("DROP TABLE STUDENTS;");
        assert(drop != nullptr);
        validator.validate(*drop);
    }

    std::cout << "Semantic validator INSERT tests passed!\n";

    return 0;
}
