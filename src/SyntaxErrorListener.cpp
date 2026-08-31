#include "SyntaxErrorListener.h"

#include <string>

void SyntaxErrorListener::syntaxError(antlr4::Recognizer *recognizer,
                                      antlr4::Token *offendingSymbol,
                                      size_t line, size_t charPositionInLine,
                                      const std::string &msg,
                                      std::exception_ptr e) {
  errors_.push_back("line " + std::to_string(line) + ": " +
                    std::to_string(charPositionInLine) + " " + msg);
}

bool SyntaxErrorListener::hasErrors() const { return !errors_.empty(); }

const std::vector<std::string> &SyntaxErrorListener::errors() const {
  return errors_;
}
