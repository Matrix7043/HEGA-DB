#include "SQLParser.h"

#include "HEGADBLexer.h"
#include "HEGADBParser.h"
#include "SyntaxErrorListener.h"

SQLParser::ParseResult SQLParser::parse(const std::string &sql) {
  antlr4::ANTLRInputStream input(sql);

  HEGADBLexer lexer(&input);

  SyntaxErrorListener lexerErrors;

  lexer.removeErrorListeners();
  lexer.addErrorListener(&lexerErrors);

  antlr4::CommonTokenStream tokens(&lexer);

  HEGADBParser parser(&tokens);

  SyntaxErrorListener parserErrors;

  parser.removeErrorListeners();
  parser.addErrorListener(&parserErrors);

  parser.query();

  ParseResult result;

  result.success = !lexerErrors.hasErrors() && !parserErrors.hasErrors();

  for (const auto &error : lexerErrors.errors()) {
    result.errors.push_back(error);
  }

  for (const auto &error : parserErrors.errors()) {
    result.errors.push_back(error);
  }

  return result;
}
