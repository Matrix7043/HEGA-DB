#include "SQLParser.h"
#include "ast/ASTPrinter.h"

#include "catalog/CatalogSnapshot.h"
#include "catalog/ColumnSchema.h"
#include "catalog/DataType.h"
#include "catalog/InMemoryCatalog.h"
#include "catalog/TableSchema.h"

#include "semantic/SemanticError.h"
#include "semantic/SemanticValidator.h"

#include <readline/history.h>
#include <readline/readline.h>

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

int main() {
  std::cout << "HEGADB Client v0.3\n";
  std::cout << "Type SQL queries. Type \\q to exit.\n\n";

  using namespace hegadb;

  // ==========================================================
  // Client Catalog
  // ==========================================================

  std::vector<ColumnSchema> studentColumns = {
      {"NAME", DataType::String, false, 0},
      {"CLASS", DataType::Integer, false, 1},
      {"GRADE", DataType::Integer, false, 2},
  };

  TableSchema students("STUDENTS", std::move(studentColumns));

  std::unordered_map<std::string, TableSchema> tables;

  tables.emplace("STUDENTS", std::move(students));

  auto snapshot = std::make_shared<const CatalogSnapshot>(1, std::move(tables));

  InMemoryCatalog catalog(snapshot);

  SemanticValidator validator(catalog);

  // ==========================================================
  // Readline
  // ==========================================================

  using_history();

  while (true) {
    char *raw_input{readline("hegadb> ")};

    if (raw_input == nullptr) {
      std::cout << '\n';
      break;
    }

    std::string input(raw_input);

    free(raw_input);

    if (input.empty()) {
      continue;
    }

    if (input == "\\q") {
      break;
    }

    add_history(input.c_str());

    // ========================================================
    // Parse
    // ========================================================

    SQLParser sqlParser;

    auto ast{sqlParser.parseToAST(input)};

    if (!ast) {
      std::cout << "Query rejected.\n";
      continue;
    }

    // ========================================================
    // Semantic validation
    // ========================================================

    try {
      validator.validate(*ast);
    } catch (const SemanticError &error) {
      std::cout << "Semantic error: " << error.what() << '\n';

      continue;
    }

    // ========================================================
    // Query accepted
    // ========================================================

    std::cout << "Query accepted.\n";
    std::cout << "AST:\n";

    ASTPrinter printer;
    printer.print(ast.get());
  }

  return 0;
}
