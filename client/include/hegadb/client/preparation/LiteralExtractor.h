#ifndef LITERAL_EXTRACTOR_H
#define LITERAL_EXTRACTOR_H

#include "hegadb/client/ast/Literal.h"

#include <cstddef>
#include <vector>

namespace hegadb::client::preparation {

struct ExtractedLiteral {

    std::size_t parameter_index;
    ast::Literal::Value value;
};

class LiteralExtractor {
public:
    std::vector<ExtractedLiteral> extract(ast::ASTNode const& root) const;

private:
    void visit(
        ast::ASTNode const& node,
        std::vector<ExtractedLiteral>& literals) const;
};

};

#endif // !LITERAL_EXTRACTOR_H
