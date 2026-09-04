#include "HEGADBLexer.h"
#include "HEGADBParser.h"
#include "ast/ASTBuilder.h"
#include "ast/ColumnRef.h"

#include "antlr4-runtime.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

using namespace antlr4;
using namespace hegadb;

int main() {
  std::string sql = "SELECT salary FROM employees;";

  ANTLRInputStream input(sql);

  HEGADBLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  HEGADBParser parser(&tokens);

  auto *tree = parser.query();

  assert(tree != nullptr);
  assert(parser.getNumberOfSyntaxErrors() == 0);

  ASTBuilder builder;

  auto result = builder.visit(tree);

  auto *stmt = std::any_cast<SelectStmt *>(result);

  assert(stmt != nullptr);

  std::unique_ptr<SelectStmt> query(stmt);

  assert(query->kind() == ASTNodeKind::SelectStmt);
  assert(query->projections().size() == 1);

  assert(query->from() != nullptr);
  assert(query->from()->name() == "employees");

  auto *column = dynamic_cast<const ColumnRef *>(query->projections()[0].get());

  assert(column != nullptr);
  assert(column->name() == "salary");

  std::cout << "ASTBuilder test passed!\n";

  return 0;
}
