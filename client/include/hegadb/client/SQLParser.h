#ifndef HEGADB_SQL_PARSER_H
#define HEGADB_SQL_PARSER_H

#include "hegadb/client/ast/SelectStmt.h"

#include <memory>
#include <string>
#include <vector>

namespace hegadb::client {

class SQLParser {
public:
    struct ParseResult {
        bool success;
        std::vector<std::string> errors;
    };

    [[nodiscard]]
    ParseResult parse(std::string const& sql);

    [[nodiscard]]
    std::unique_ptr<ast::SelectStmt> parseToAST(std::string const& sql);
};

}

#endif
