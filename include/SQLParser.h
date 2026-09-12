#ifndef HEGADB_SQL_PARSER_H
#define HEGADB_SQL_PARSER_H

#include "ast/SelectStmt.h"
#include <string>
#include <vector>

class SQLParser {
public:
    struct ParseResult {
        bool success;
        std::vector<std::string> errors;
    };

    [[nodiscard]]
    ParseResult parse(std::string const& sql);

    [[nodiscard]]
    std::unique_ptr<hegadb::SelectStmt> parseToAST(std::string const& sql);
};

#endif
