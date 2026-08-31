#ifndef HEGADB_SQL_PARSER_H
#define HEGADB_SQL_PARSER_H

#include <string>
#include <vector>

class SQLParser {
public:
  struct ParseResult {
    bool success;
    std::vector<std::string> errors;
  };

  [[nodiscard]]
  ParseResult parse(const std::string &sql);
};

#endif
