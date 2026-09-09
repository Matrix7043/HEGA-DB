#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/ColumnRef.h"
#include "ast/SelectStmt.h"
#include "ast/TableRef.h"
#include "ast/Wildcard.h"

#include "catalog/InMemoryCatalog.h"

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

  // --------------------------------------------------------
  // Valid query:
  // SELECT * FROM STUDENTS;
  // --------------------------------------------------------

  {
    SelectStmt stmt;

    stmt.addProjection(std::make_unique<Wildcard>());

    stmt.setFrom(std::make_unique<TableRef>("STUDENTS"));

    validator.validate(stmt);
  }

  // --------------------------------------------------------
  // Valid query:
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

  std::cout << "All semantic validator tests passed.\n";

  return 0;
}
