#ifndef LITERAL_EXTRACTOR_H
#define LITERAL_EXTRACTOR_H

#include "ast/Literal.h"
#include <cstddef>
#include <vector>

namespace hegadb {

struct ExtractedLiteral {

    std::size_t parameter_index;
    Literal::Value value;
};

class LiteralExtractor {
public:
    std::vector<ExtractedLiteral> extract(ASTNode const& root) const;

private:
    void visit(
        ASTNode const& node,
        std::vector<ExtractedLiteral>& literals) const;
};

};

#endif // !LITERAL_EXTRACTOR_H
