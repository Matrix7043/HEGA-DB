#include "ast/ColumnRef.h"
#include "ast/ComparisonExpr.h"
#include "ast/Literal.h"
#include "ast/LogicalExpr.h"
#include "ast/SelectStmt.h"
#include "ast/TableRef.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

using namespace hegadb;

int main() {
  auto query = std::make_unique<SelectStmt>();

  query->addProjection(std::make_unique<ColumnRef>("salary"));

  query->addProjection(std::make_unique<ColumnRef>("name"));

  query->setFrom(std::make_unique<TableRef>("employees"));

  auto salaryComparison = std::make_unique<ComparisonExpr>(
      std::make_unique<ColumnRef>("salary"), ComparisonOp::GT,
      std::make_unique<Literal>(50000LL));

  auto nameComparison = std::make_unique<ComparisonExpr>(
      std::make_unique<ColumnRef>("name"), ComparisonOp::EQ,
      std::make_unique<Literal>(std::string("Bob")));

  auto where = std::make_unique<LogicalExpr>(
      std::move(salaryComparison), LogicalOp::AND, std::move(nameComparison));

  query->setWhere(std::move(where));

  assert(query->projections().size() == 2);
  assert(query->from() != nullptr);
  assert(query->from()->name() == "employees");
  assert(query->where() != nullptr);

  std::cout << "AST test passed!\n";

  return 0;
}
