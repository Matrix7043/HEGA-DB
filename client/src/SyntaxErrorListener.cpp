#include "hegadb/client/SyntaxErrorListener.h"

#include <string>

namespace hegadb::client {

void SyntaxErrorListener::syntaxError([[maybe_unused]] antlr4::Recognizer* recognizer,
    [[maybe_unused]] antlr4::Token* offendingSymbol,
    size_t line, size_t charPositionInLine,
    std::string const& msg,
    [[maybe_unused]] std::exception_ptr e)
{
    errors_.push_back("line " + std::to_string(line) + ": " + std::to_string(charPositionInLine) + " " + msg);
}

bool SyntaxErrorListener::hasErrors() const { return !errors_.empty(); }

std::vector<std::string> const& SyntaxErrorListener::errors() const
{
    return errors_;
}

}
