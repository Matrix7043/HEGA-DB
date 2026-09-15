#include "hegadb/client/SQLParser.h"
#include "hegadb/client/SyntaxErrorListener.h"

#include "HEGADBLexer.h"
#include "HEGADBParser.h"
#include "hegadb/client/ast/ASTBuilder.h"
#include "hegadb/client/ast/SelectStmt.h"
#include <memory>

namespace hegadb::client {

SQLParser::ParseResult SQLParser::parse(std::string const& sql)
{
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

    for (auto const& error : lexerErrors.errors()) {
        result.errors.push_back(error);
    }

    for (auto const& error : parserErrors.errors()) {
        result.errors.push_back(error);
    }

    return result;
}

std::unique_ptr<ast::SelectStmt>
SQLParser::parseToAST(std::string const& sql)
{

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

    auto* tree = parser.query();

    if (lexerErrors.hasErrors() || parserErrors.hasErrors()) {
        return nullptr;
    }

    ast::ASTBuilder builder;

    auto result = builder.visitQuery(tree);

    auto* stmt = std::any_cast<ast::SelectStmt*>(result);

    return std::unique_ptr<ast::SelectStmt>(stmt);
}

}
