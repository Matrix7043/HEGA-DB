#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/SelectStmt.h>
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

using namespace hegadb;
using client::SQLParser;
using client::semantic::SemanticError;
using client::semantic::SemanticErrorKind;
using client::semantic::SemanticValidator;
using types::DataType;

namespace {

// STUDENTS and ENROLLMENTS both have a CLASS column (for ambiguity checks).
catalog::InMemoryCatalog createCatalog()
{
    catalog::TableSchema students(
        "STUDENTS",
        { { "ID", DataType::Integer, false, 0 },
            { "NAME", DataType::String, false, 1 },
            { "CLASS", DataType::Integer, false, 2 } });

    catalog::TableSchema enrollments(
        "ENROLLMENTS",
        { { "STUDENT_ID", DataType::Integer, false, 0 },
            { "COURSE", DataType::String, false, 1 },
            { "CLASS", DataType::Integer, false, 2 },
            { "SCORE", DataType::Float, false, 3 } });

    std::unordered_map<std::string, catalog::TableSchema> tables;
    tables.emplace("STUDENTS", std::move(students));
    tables.emplace("ENROLLMENTS", std::move(enrollments));

    return catalog::InMemoryCatalog(
        std::make_shared<catalog::CatalogSnapshot const>(1, std::move(tables)));
}

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

} // namespace

int main()
{
    auto catalog = createCatalog();
    SemanticValidator validator(catalog);
    SQLParser parser;

    auto parse = [&](std::string const& sql) {
        auto ast = parser.parseToAST(sql);
        assert(ast != nullptr);
        return ast;
    };

    auto valid = [&](std::string const& sql) {
        auto ast = parse(sql);
        validator.validate(*ast);
    };

    auto invalid = [&](SemanticErrorKind kind, std::string const& sql) {
        auto ast = parse(sql);
        expectError(kind, [&] { validator.validate(*ast); });
    };

    // ---------------- Valid joins ----------------

    valid("SELECT * FROM STUDENTS INNER JOIN ENROLLMENTS "
          "ON STUDENTS.ID = ENROLLMENTS.STUDENT_ID;");

    // ON columns in either order
    valid("SELECT * FROM STUDENTS JOIN ENROLLMENTS "
          "ON ENROLLMENTS.STUDENT_ID = STUDENTS.ID;");

    // unqualified (unique) and qualified (ambiguous name) projections
    valid("SELECT NAME, SCORE, STUDENTS.CLASS, ENROLLMENTS.CLASS "
          "FROM STUDENTS JOIN ENROLLMENTS ON STUDENTS.ID = ENROLLMENTS.STUDENT_ID;");

    // join on a column name both tables share, same type
    valid("SELECT * FROM STUDENTS JOIN ENROLLMENTS "
          "ON STUDENTS.CLASS = ENROLLMENTS.CLASS;");

    // ---------------- Tables ----------------

    invalid(SemanticErrorKind::TableNotFound,
        "SELECT * FROM NOPE JOIN ENROLLMENTS ON NOPE.ID = ENROLLMENTS.STUDENT_ID;");

    invalid(SemanticErrorKind::TableNotFound,
        "SELECT * FROM STUDENTS JOIN NOPE ON STUDENTS.ID = NOPE.ID;");

    // no aliases, so a table cannot be joined with itself
    invalid(SemanticErrorKind::InvalidJoin,
        "SELECT * FROM STUDENTS JOIN STUDENTS ON STUDENTS.ID = STUDENTS.CLASS;");

    // ---------------- ON condition ----------------

    // both sides from the same table
    invalid(SemanticErrorKind::InvalidJoin,
        "SELECT * FROM STUDENTS JOIN ENROLLMENTS ON STUDENTS.ID = STUDENTS.CLASS;");

    // a column that does not exist
    invalid(SemanticErrorKind::ColumnNotFound,
        "SELECT * FROM STUDENTS JOIN ENROLLMENTS ON STUDENTS.NOPE = ENROLLMENTS.STUDENT_ID;");

    // a table that is not part of the query
    invalid(SemanticErrorKind::TableNotFound,
        "SELECT * FROM STUDENTS JOIN ENROLLMENTS ON STUDENTS.ID = OTHER.STUDENT_ID;");

    // different types (String vs Integer, Integer vs Float)
    invalid(SemanticErrorKind::TypeMismatch,
        "SELECT * FROM STUDENTS JOIN ENROLLMENTS ON STUDENTS.NAME = ENROLLMENTS.STUDENT_ID;");

    invalid(SemanticErrorKind::TypeMismatch,
        "SELECT * FROM STUDENTS JOIN ENROLLMENTS ON STUDENTS.ID = ENROLLMENTS.SCORE;");

    // ---------------- Projections ----------------

    // CLASS exists in both tables
    invalid(SemanticErrorKind::AmbigousColumn,
        "SELECT CLASS FROM STUDENTS JOIN ENROLLMENTS ON STUDENTS.ID = ENROLLMENTS.STUDENT_ID;");

    invalid(SemanticErrorKind::ColumnNotFound,
        "SELECT NOPE FROM STUDENTS JOIN ENROLLMENTS ON STUDENTS.ID = ENROLLMENTS.STUDENT_ID;");

    // column exists, but not in the table it is qualified with
    invalid(SemanticErrorKind::ColumnNotFound,
        "SELECT STUDENTS.SCORE FROM STUDENTS JOIN ENROLLMENTS ON STUDENTS.ID = ENROLLMENTS.STUDENT_ID;");

    invalid(SemanticErrorKind::TableNotFound,
        "SELECT OTHER.ID FROM STUDENTS JOIN ENROLLMENTS ON STUDENTS.ID = ENROLLMENTS.STUDENT_ID;");

    // ---------------- WHERE + JOIN (only possible with a hand-built AST) ----------------

    {
        auto ast = parse("SELECT * FROM STUDENTS WHERE ID = 1;");
        auto joined = parse("SELECT * FROM STUDENTS JOIN ENROLLMENTS "
                            "ON STUDENTS.ID = ENROLLMENTS.STUDENT_ID;");

        auto* select = dynamic_cast<client::ast::SelectStmt*>(ast.get());
        auto* joined_select = dynamic_cast<client::ast::SelectStmt*>(joined.get());
        assert(select != nullptr && joined_select != nullptr);

        // move the JOIN clause into the statement that already has a WHERE
        select->setJoin(std::make_unique<client::ast::JoinClause>(
            std::make_unique<client::ast::TableRef>(joined_select->join()->table()->name()),
            std::make_unique<client::ast::ColumnRef>(
                *joined_select->join()->left()->table(), joined_select->join()->left()->name()),
            std::make_unique<client::ast::ColumnRef>(
                *joined_select->join()->right()->table(), joined_select->join()->right()->name())));

        expectError(SemanticErrorKind::InvalidJoin,
            [&] { validator.validate(*ast); });
    }

    // ---------------- Qualified columns without a JOIN ----------------

    valid("SELECT STUDENTS.NAME FROM STUDENTS;");

    invalid(SemanticErrorKind::TableNotFound,
        "SELECT ENROLLMENTS.SCORE FROM STUDENTS;");

    // ---------------- Plain SELECT is unchanged ----------------

    valid("SELECT * FROM STUDENTS WHERE ID = 1;");
    valid("SELECT NAME FROM STUDENTS;");

    std::cout << "Semantic validator JOIN tests passed!\n";

    return 0;
}
