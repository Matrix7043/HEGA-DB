#include "SQLParser.h"
#include "ast/ASTPrinter.h"

#include <readline/history.h>
#include <readline/readline.h>

#include <iostream>
#include <string>

int main() {
  std::cout << "HEGADB Client v0.1\n";
  std::cout << "Type SQL queries. Type \\q to exit.\n\n";

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

    SQLParser sqlParser;

    auto ast{sqlParser.parseToAST(input)};

    if (ast) {
      std::cout << "Query accepted.\n";
      std::cout << "AST:\n";

      hegadb::ASTPrinter printer;
      printer.print(ast.get());
    } else {
      std::cout << "Query rejected.\n";
    }
  }

  return 0;
}
