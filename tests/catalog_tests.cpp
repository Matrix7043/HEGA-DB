#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "catalog/CatalogSnapshot.h"
#include "catalog/ColumnSchema.h"
#include "catalog/DataType.h"
#include "catalog/InMemoryCatalog.h"
#include "catalog/TableSchema.h"

using namespace hegadb;

int main() {

  // --------------------------------------------------------
  // Create STUDENTS table schema
  // --------------------------------------------------------

  std::vector<ColumnSchema> columns = {{"NAME", DataType::String, false, 0},
                                       {"CLASS", DataType::Integer, false, 1},
                                       {"GRADE", DataType::Integer, false, 2}};

  TableSchema students("STUDENTS", columns);

  // --------------------------------------------------------
  // Create catalog snapshot
  // --------------------------------------------------------

  std::unordered_map<std::string, TableSchema> tables;

  tables.emplace("STUDENTS", std::move(students));

  auto snapshot = std::make_shared<const CatalogSnapshot>(1, std::move(tables));

  // --------------------------------------------------------
  // Create catalog
  // --------------------------------------------------------

  InMemoryCatalog catalog(snapshot);

  // --------------------------------------------------------
  // Test schema version
  // --------------------------------------------------------

  assert(catalog.version() == 1);

  // --------------------------------------------------------
  // Test table lookup
  // --------------------------------------------------------

  const TableSchema *table = catalog.findTable("STUDENTS");

  assert(table != nullptr);
  assert(table->name() == "STUDENTS");

  // --------------------------------------------------------
  // Test column lookup
  // --------------------------------------------------------

  const ColumnSchema *nameColumn = table->findColumn("NAME");

  assert(nameColumn != nullptr);
  assert(nameColumn->name == "NAME");
  assert(nameColumn->type == DataType::String);

  const ColumnSchema *classColumn = table->findColumn("CLASS");

  assert(classColumn != nullptr);
  assert(classColumn->name == "CLASS");
  assert(classColumn->type == DataType::Integer);

  const ColumnSchema *gradeColumn = table->findColumn("GRADE");

  assert(gradeColumn != nullptr);
  assert(gradeColumn->name == "GRADE");
  assert(gradeColumn->type == DataType::Integer);

  // --------------------------------------------------------
  // Test missing column
  // --------------------------------------------------------

  const ColumnSchema *missingColumn = table->findColumn("DOES_NOT_EXIST");

  assert(missingColumn == nullptr);

  // --------------------------------------------------------
  // Test missing table
  // --------------------------------------------------------

  const TableSchema *missingTable = catalog.findTable("DOES_NOT_EXIST");

  assert(missingTable == nullptr);

  std::cout << "All catalog tests passed.\n";

  return 0;
}
