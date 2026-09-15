#ifndef HEGADB_SYNTAX_ERROR_LISTENER_H
#define HEGADB_SYNTAX_ERROR_LISTENER_H

#include "antlr4-runtime.h"
#include <string>

#include <string>
#include <vector>

namespace hegadb::client {

class SyntaxErrorListener final : public antlr4::BaseErrorListener {
public:
    void syntaxError(antlr4::Recognizer* recognizer,
        antlr4::Token* offendingSymbol, size_t line,
        size_t charPositionInLLine, std::string const& msg,
        std::exception_ptr e) override;

    [[nodiscard]]
    bool hasErrors() const;

    [[nodiscard]]
    std::vector<std::string> const& errors() const;

private:
    std::vector<std::string> errors_;
};

}

#endif
