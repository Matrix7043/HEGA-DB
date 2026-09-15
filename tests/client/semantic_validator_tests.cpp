#include <hegadb/client/ast/ColumnRef.h>
#include <hegadb/client/ast/ComparisonExpr.h>
#include <hegadb/client/ast/Literal.h>
#include <hegadb/client/ast/LogicalExpr.h>
#include <hegadb/client/ast/NotExpr.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/ast/TableRef.h>
#include <hegadb/client/ast/Wildcard.h>

#include <hegadb/client/semantic/SemanticError.h>
#include <hegadb/client/semantic/SemanticValidator.h>

#include <hegadb/catalog/CatalogSnapshot.h>
#include <hegadb/catalog/ColumnSchema.h>
#include <hegadb/catalog/InMemoryCatalog.h>
#include <hegadb/catalog/TableSchema.h>

#include <hegadb/operations/ComparisonOp.h>
#include <hegadb/operations/LogicalOp.h>

#include <hegadb/types/DataType.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

static hegadb::catalog::InMemoryCatalog createCatalog()
{
    std::vector<hegadb::catalog::ColumnSchema> columns = {
        { "NAME",
            hegadb::types::DataType::String,
            false,
            0 },
        { "CLASS",
            hegadb::types::DataType::Integer,
            false,
            1 },
        { "GRADE",
            hegadb::types::DataType::Integer,
            false,
            2 }
    };

    hegadb::catalog::TableSchema students(
        "STUDENTS",
        columns);

    std::unordered_map<
        std::string,
        hegadb::catalog::TableSchema>
        tables;

    tables.emplace(
        "STUDENTS",
        std::move(students));

    auto snapshot = std::make_shared<
        hegadb::catalog::CatalogSnapshot const>(1, std::move(tables));

    return hegadb::catalog::InMemoryCatalog(snapshot);
}

int main()
{
    auto catalog = createCatalog();

    hegadb::client::semantic::SemanticValidator validator(catalog);

    // Valid table
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::Wildcard>());

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("STUDENTS"));

        validator.validate(stmt);
    }

    // Valid column
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::ColumnRef>("NAME"));

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("STUDENTS"));

        validator.validate(stmt);
    }

    // Invalid table
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::Wildcard>());

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("UNKNOWN"));

        bool failed = false;

        try {
            validator.validate(stmt);
        } catch (
            hegadb::client::semantic::SemanticError const& error) {

            failed = true;

            assert(
                error.kind() == hegadb::client::semantic::SemanticErrorKind::TableNotFound);
        }

        assert(failed);
    }

    // Invalid column
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::ColumnRef>("AGE"));

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("STUDENTS"));

        bool failed = false;

        try {
            validator.validate(stmt);
        } catch (
            hegadb::client::semantic::SemanticError const& error) {

            failed = true;

            assert(
                error.kind() == hegadb::client::semantic::SemanticErrorKind::ColumnNotFound);
        }

        assert(failed);
    }

    // Valid integer comparison
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::ColumnRef>("NAME"));

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("STUDENTS"));

        stmt.setWhere(
            std::make_unique<
                hegadb::client::ast::ComparisonExpr>(
                std::make_unique<
                    hegadb::client::ast::ColumnRef>("GRADE"),
                hegadb::operations::ComparisonOp::GT,
                std::make_unique<
                    hegadb::client::ast::Literal>(100LL)));

        validator.validate(stmt);
    }

    // Valid string comparison
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::Wildcard>());

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("STUDENTS"));

        stmt.setWhere(
            std::make_unique<
                hegadb::client::ast::ComparisonExpr>(
                std::make_unique<
                    hegadb::client::ast::ColumnRef>("NAME"),
                hegadb::operations::ComparisonOp::EQ,
                std::make_unique<
                    hegadb::client::ast::Literal>(
                    std::string("Alice"))));

        validator.validate(stmt);
    }

    // Valid boolean comparison
    {
        std::vector<hegadb::catalog::ColumnSchema> columns = {
            { "ACTIVE",
                hegadb::types::DataType::Boolean,
                false,
                0 }
        };

        hegadb::catalog::TableSchema table(
            "USERS",
            columns);

        std::unordered_map<
            std::string,
            hegadb::catalog::TableSchema>
            tables;

        tables.emplace(
            "USERS",
            std::move(table));

        auto snapshot = std::make_shared<
            hegadb::catalog::CatalogSnapshot const>(2, std::move(tables));

        hegadb::catalog::InMemoryCatalog boolCatalog(snapshot);

        hegadb::client::semantic::SemanticValidator boolValidator(
            boolCatalog);

        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::Wildcard>());

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("USERS"));

        stmt.setWhere(
            std::make_unique<
                hegadb::client::ast::ComparisonExpr>(
                std::make_unique<
                    hegadb::client::ast::ColumnRef>("ACTIVE"),
                hegadb::operations::ComparisonOp::EQ,
                std::make_unique<
                    hegadb::client::ast::Literal>(true)));

        boolValidator.validate(stmt);
    }

    // Invalid comparison
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::ColumnRef>("NAME"));

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("STUDENTS"));

        stmt.setWhere(
            std::make_unique<
                hegadb::client::ast::ComparisonExpr>(
                std::make_unique<
                    hegadb::client::ast::ColumnRef>("NAME"),
                hegadb::operations::ComparisonOp::GT,
                std::make_unique<
                    hegadb::client::ast::Literal>(100LL)));

        bool failed = false;

        try {
            validator.validate(stmt);
        } catch (
            hegadb::client::semantic::SemanticError const& error) {

            failed = true;

            assert(
                error.kind() == hegadb::client::semantic::SemanticErrorKind::TypeMismatch);
        }

        assert(failed);
    }

    // Valid AND
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::Wildcard>());

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("STUDENTS"));

        auto gradeCheck = std::make_unique<
            hegadb::client::ast::ComparisonExpr>(
            std::make_unique<
                hegadb::client::ast::ColumnRef>("GRADE"),
            hegadb::operations::ComparisonOp::GT,
            std::make_unique<
                hegadb::client::ast::Literal>(50LL));

        auto classCheck = std::make_unique<
            hegadb::client::ast::ComparisonExpr>(
            std::make_unique<
                hegadb::client::ast::ColumnRef>("CLASS"),
            hegadb::operations::ComparisonOp::GT,
            std::make_unique<
                hegadb::client::ast::Literal>(10LL));

        stmt.setWhere(
            std::make_unique<
                hegadb::client::ast::LogicalExpr>(
                std::move(gradeCheck),
                hegadb::operations::LogicalOp::AND,
                std::move(classCheck)));

        validator.validate(stmt);
    }

    // Valid OR
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::Wildcard>());

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("STUDENTS"));

        auto gradeCheck = std::make_unique<
            hegadb::client::ast::ComparisonExpr>(
            std::make_unique<
                hegadb::client::ast::ColumnRef>("GRADE"),
            hegadb::operations::ComparisonOp::GT,
            std::make_unique<
                hegadb::client::ast::Literal>(90LL));

        auto classCheck = std::make_unique<
            hegadb::client::ast::ComparisonExpr>(
            std::make_unique<
                hegadb::client::ast::ColumnRef>("CLASS"),
            hegadb::operations::ComparisonOp::GT,
            std::make_unique<
                hegadb::client::ast::Literal>(12LL));

        stmt.setWhere(
            std::make_unique<
                hegadb::client::ast::LogicalExpr>(
                std::move(gradeCheck),
                hegadb::operations::LogicalOp::OR,
                std::move(classCheck)));

        validator.validate(stmt);
    }

    // Invalid logical expression
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::Wildcard>());

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("STUDENTS"));

        stmt.setWhere(
            std::make_unique<
                hegadb::client::ast::LogicalExpr>(
                std::make_unique<
                    hegadb::client::ast::ColumnRef>("GRADE"),
                hegadb::operations::LogicalOp::AND,
                std::make_unique<
                    hegadb::client::ast::ColumnRef>("CLASS")));

        bool failed = false;

        try {
            validator.validate(stmt);
        } catch (
            hegadb::client::semantic::SemanticError const& error) {

            failed = true;

            assert(
                error.kind() == hegadb::client::semantic::SemanticErrorKind::TypeMismatch);
        }

        assert(failed);
    }

    // Valid NOT
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::Wildcard>());

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("STUDENTS"));

        auto comparison = std::make_unique<
            hegadb::client::ast::ComparisonExpr>(
            std::make_unique<
                hegadb::client::ast::ColumnRef>("GRADE"),
            hegadb::operations::ComparisonOp::GT,
            std::make_unique<
                hegadb::client::ast::Literal>(50LL));

        stmt.setWhere(
            std::make_unique<
                hegadb::client::ast::NotExpr>(
                std::move(comparison)));

        validator.validate(stmt);
    }

    // Invalid NOT
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::Wildcard>());

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("STUDENTS"));

        stmt.setWhere(
            std::make_unique<
                hegadb::client::ast::NotExpr>(
                std::make_unique<
                    hegadb::client::ast::ColumnRef>("GRADE")));

        bool failed = false;

        try {
            validator.validate(stmt);
        } catch (
            hegadb::client::semantic::SemanticError const& error) {

            failed = true;

            assert(
                error.kind() == hegadb::client::semantic::SemanticErrorKind::TypeMismatch);
        }

        assert(failed);
    }

    // Invalid WHERE expression
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::Wildcard>());

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("STUDENTS"));

        stmt.setWhere(
            std::make_unique<
                hegadb::client::ast::ColumnRef>("GRADE"));

        bool failed = false;

        try {
            validator.validate(stmt);
        } catch (
            hegadb::client::semantic::SemanticError const& error) {

            failed = true;

            assert(
                error.kind() == hegadb::client::semantic::SemanticErrorKind::TypeMismatch);
        }

        assert(failed);
    }

    // Valid Boolean WHERE
    {
        hegadb::client::ast::SelectStmt stmt;

        stmt.addProjection(
            std::make_unique<
                hegadb::client::ast::Wildcard>());

        stmt.setFrom(
            std::make_unique<
                hegadb::client::ast::TableRef>("STUDENTS"));

        stmt.setWhere(
            std::make_unique<
                hegadb::client::ast::Literal>(true));

        validator.validate(stmt);
    }

    std::cout
        << "All semantic validator tests passed.\n";

    return 0;
}
