#include "antlr4-runtime.h"

#include "HEGADBLexer.h"
#include "HEGADBParser.h"

#include <iostream>
#include <string>

int main() {

  const std::string sql = "SELECT name, salary "
                          "FROM employees "
                          "WHERE salary >= 50000 AND active = TRUE;";

  std::cout << "SQL:\n";
  std::cout << sql << "\n\n";

  antlr4::ANTLRInputStream input(sql);

  HEGADBLexer lexer(&input);

  antlr4::CommonTokenStream tokens(&lexer);

  HEGADBParser parser(&tokens);

  antlr4::tree::ParseTree *tree = parser.query();

  std::cout << "Parse tree:\n";
  std::cout << tree->toStringTree(&parser) << "\n";

  return 0;
}
