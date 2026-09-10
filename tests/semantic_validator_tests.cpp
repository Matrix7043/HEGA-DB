#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/ColumnRef.h"
#include "ast/ComparisonExpr.h"
#include "ast/Literal.h"
#include "ast/LogicalExpr.h"
#include "ast/NotExpr.h"
#include "ast/SelectStmt.h"
#include "ast/TableRef.h"
#include "ast/Wildcard.h"

#include "catalog/CatalogSnapshot.h"
#include "catalog/ColumnSchema.h"
#include "catalog/DataType.h"
#include "catalog/InMemoryCatalog.h"
#include "catalog/TableSchema.h"

#include "semantic/SemanticError.h"
#include "semantic/SemanticValidator.h"

using namespace hegadb;

static InMemoryCatalog createCatalog() {

  std::vector<ColumnSchema> columns = {
      {"NAME", DataType::String, false, 0},
      {"CLASS", DataType::Integer, false, 1},
      {"GRADE", DataType::Integer, false, 2},
  };

  TableSchema students("STUDENTS", columns);

  std::unordered_map<std::string, TableSchema> tables;

  tables.emplace("STUDENTS", std::move(students));

  auto snapshot = std::make_shared<const CatalogSnapshot>(1, std::move(tables));

  return InMemoryCatalog(snapshot);
}

int main() {

  auto catalog = createCatalog();
  SemanticValidator validator(catalog);

  // ========================================================
  // Table / Column Resolution
  // ========================================================

  // --------------------------------------------------------
  // Valid table
  // SELECT * FROM STUDENTS;
  // --------------------------------------------------------

  {
    SelectStmt stmt;

    stmt.addProjection(std::make_unique<Wildcard>());

    stmt.setFrom(std::make_unique<TableRef>("STUDENTS"));

    validator.validate(stmt);
  }

  // --------------------------------------------------------
  // Valid column
  // SELECT NAME FROM STUDENTS;
  // --------------------------------------------------------

  {
    SelectStmt stmt;

    stmt.addProjection(std::make_unique<ColumnRef>("NAME"));

    stmt.setFrom(std::make_unique<TableRef>("STUDENTS"));

    validator.validate(stmt);
  }

  // --------------------------------------------------------
  // Invalid table
  // SELECT * FROM UNKNOWN;
  // --------------------------------------------------------

  {
    SelectStmt stmt;

    stmt.addProjection(std::make_unique<Wildcard>());

    stmt.setFrom(std::make_unique<TableRef>("UNKNOWN"));

    bool failed = false;

    try {
      validator.validate(stmt);
    } catch (const SemanticError &error) {

      failed = true;

      assert(error.kind() == SemanticErrorKind::TableNotFound);
    }

    assert(failed);
  }

  // --------------------------------------------------------
  // Invalid column
  // SELECT AGE FROM STUDENTS;
  // --------------------------------------------------------

  {
    SelectStmt stmt;

    stmt.addProjection(std::make_unique<ColumnRef>("AGE"));

    stmt.setFrom(std::make_unique<TableRef>("STUDENTS"));

    bool failed = false;

    try {
      validator.validate(stmt);
    } catch (const SemanticError &error) {

      failed = true;

      assert(error.kind() == SemanticErrorKind::ColumnNotFound);
    }

    assert(failed);
  }

  // ========================================================
  // Comparison Type Checking
  // ========================================================

  // --------------------------------------------------------
  // Valid integer comparison
  // SELECT NAME
  // FROM STUDENTS
  // WHERE GRADE > 100;
  // --------------------------------------------------------

  {
    SelectStmt stmt;

    stmt.addProjection(std::make_unique<ColumnRef>("NAME"));

    stmt.setFrom(std::make_unique<TableRef>("STUDENTS"));

    stmt.setWhere(std::make_unique<ComparisonExpr>(
        std::make_unique<ColumnRef>("GRADE"), ComparisonOp::GT,
        std::make_unique<Literal>(100LL)));

    validator.validate(stmt);
  }

  // --------------------------------------------------------
  // Valid string comparison
  // WHERE NAME = 'Alice';
  // --------------------------------------------------------

  {
    SelectStmt stmt;

    stmt.addProjection(std::make_unique<Wildcard>());

    stmt.setFrom(std::make_unique<TableRef>("STUDENTS"));

    stmt.setWhere(std::make_unique<ComparisonExpr>(
        std::make_unique<ColumnRef>("NAME"), ComparisonOp::EQ,
        std::make_unique<Literal>(std::string("Alice"))));

    validator.validate(stmt);
  }

  // --------------------------------------------------------
  // Valid boolean comparison
  //
  // This directly tests the Literal(bool) path.
  // --------------------------------------------------------

  {
    std::vector<ColumnSchema> columns = {
        {"ACTIVE", DataType::Boolean, false, 0},
    };

    TableSchema table("USERS", columns);

    std::unordered_map<std::string, TableSchema> tables;
    tables.emplace("USERS", std::move(table));

    auto snapshot =
        std::make_shared<const CatalogSnapshot>(2, std::move(tables));

    InMemoryCatalog boolCatalog(snapshot);

    SemanticValidator boolValidator(boolCatalog);

    SelectStmt stmt;

    stmt.addProjection(std::make_unique<Wildcard>());

    stmt.setFrom(std::make_unique<TableRef>("USERS"));

    stmt.setWhere(std::make_unique<ComparisonExpr>(
        std::make_unique<ColumnRef>("ACTIVE"), ComparisonOp::EQ,
        std::make_unique<Literal>(true)));

    boolValidator.validate(stmt);
  }

  // --------------------------------------------------------
  // Invalid comparison
  //
  // WHERE NAME > 100;
  //
  // String > Integer
  // --------------------------------------------------------

  {
    SelectStmt stmt;

    stmt.addProjection(std::make_unique<ColumnRef>("NAME"));

    stmt.setFrom(std::make_unique<TableRef>("STUDENTS"));

    stmt.setWhere(std::make_unique<ComparisonExpr>(
        std::make_unique<ColumnRef>("NAME"), ComparisonOp::GT,
        std::make_unique<Literal>(100LL)));

    bool failed = false;

    try {
      validator.validate(stmt);
    } catch (const SemanticError &error) {

      failed = true;

      assert(error.kind() == SemanticErrorKind::TypeMismatch);
    }

    assert(failed);
  }

  // ========================================================
  // Logical Expression Checking
  // ========================================================

  // --------------------------------------------------------
  // Valid AND
  //
  // WHERE GRADE > 50 AND CLASS > 10;
  // --------------------------------------------------------

  {
    SelectStmt stmt;

    stmt.addProjection(std::make_unique<Wildcard>());

    stmt.setFrom(std::make_unique<TableRef>("STUDENTS"));

    auto gradeCheck = std::make_unique<ComparisonExpr>(
        std::make_unique<ColumnRef>("GRADE"), ComparisonOp::GT,
        std::make_unique<Literal>(50LL));

    auto classCheck = std::make_unique<ComparisonExpr>(
        std::make_unique<ColumnRef>("CLASS"), ComparisonOp::GT,
        std::make_unique<Literal>(10LL));

    stmt.setWhere(std::make_unique<LogicalExpr>(
        std::move(gradeCheck), LogicalOp::AND, std::move(classCheck)));

    validator.validate(stmt);
  }

  // --------------------------------------------------------
  // Valid OR
  //
  // WHERE GRADE > 90 OR CLASS > 12;
  // --------------------------------------------------------

  {
    SelectStmt stmt;

    stmt.addProjection(std::make_unique<Wildcard>());

    stmt.setFrom(std::make_unique<TableRef>("STUDENTS"));

    auto gradeCheck = std::make_unique<ComparisonExpr>(
        std::make_unique<ColumnRef>("GRADE"), ComparisonOp::GT,
        std::make_unique<Literal>(90LL));

    auto classCheck = std::make_unique<ComparisonExpr>(
        std::make_unique<ColumnRef>("CLASS"), ComparisonOp::GT,
        std::make_unique<Literal>(12LL));

    stmt.setWhere(std::make_unique<LogicalExpr>(
        std::move(gradeCheck), LogicalOp::OR, std::move(classCheck)));

    validator.validate(stmt);
  }

  // --------------------------------------------------------
  // Invalid logical expression
  //
  // WHERE GRADE AND CLASS;
  //
  // Integer AND Integer
  // --------------------------------------------------------

  {
    SelectStmt stmt;

    stmt.addProjection(std::make_unique<Wildcard>());

    stmt.setFrom(std::make_unique<TableRef>("STUDENTS"));

    stmt.setWhere(std::make_unique<LogicalExpr>(
        std::make_unique<ColumnRef>("GRADE"), LogicalOp::AND,
        std::make_unique<ColumnRef>("CLASS")));

    bool failed = false;

    try {
      validator.validate(stmt);
    } catch (const SemanticError &error) {

      failed = true;

      assert(error.kind() == SemanticErrorKind::TypeMismatch);
    }

    assert(failed);
  }

  // ========================================================
  // NOT Expression Checking
  // ========================================================

  // --------------------------------------------------------
  // Valid NOT
  //
  // WHERE NOT (GRADE > 50);
  // --------------------------------------------------------

  {
    SelectStmt stmt;

    stmt.addProjection(std::make_unique<Wildcard>());

    stmt.setFrom(std::make_unique<TableRef>("STUDENTS"));

    auto comparison = std::make_unique<ComparisonExpr>(
        std::make_unique<ColumnRef>("GRADE"), ComparisonOp::GT,
        std::make_unique<Literal>(50LL));

    stmt.setWhere(std::make_unique<NotExpr>(std::move(comparison)));

    validator.validate(stmt);
  }

  // --------------------------------------------------------
  // Invalid NOT
  //
  // WHERE NOT GRADE;
  //
  // NOT Integer
  // --------------------------------------------------------

  {
    SelectStmt stmt;

    stmt.addProjection(std::make_unique<Wildcard>());

    stmt.setFrom(std::make_unique<TableRef>("STUDENTS"));

    stmt.setWhere(
        std::make_unique<NotExpr>(std::make_unique<ColumnRef>("GRADE")));

    bool failed = false;

    try {
      validator.validate(stmt);
    } catch (const SemanticError &error) {

      failed = true;

      assert(error.kind() == SemanticErrorKind::TypeMismatch);
    }

    assert(failed);
  }

  std::cout << "All semantic validator tests passed.\n";

  return 0;
}
