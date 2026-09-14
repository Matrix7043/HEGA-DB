#include "ast/ASTNode.h"
#include "ast/ComparisonExpr.h"
#include "ast/LogicalExpr.h"
#include "ast/NotExpr.h"
#include "ast/SelectStmt.h"
#include "preparation/LiteralExtractor.h"
#include <vector>

namespace hegadb {

void LiteralExtractor::visit(
    ASTNode const& node,
    std::vector<ExtractedLiteral>& literals) const
{
    if (auto const* selectStmt = dynamic_cast<SelectStmt const*>(&node)) {
        if (selectStmt->where()) {
            visit(*selectStmt->where(), literals);
        }
        return;
    }

    if (auto const* literal = dynamic_cast<Literal const*>(&node)) {

        literals.push_back({ literals.size(), literal->value() });
        return;
    }

    if (auto const* comparison = dynamic_cast<ComparisonExpr const*>(&node)) {
        if (comparison->left()) {
            visit(*comparison->left(), literals);
        }
        if (comparison->right()) {
            visit(*comparison->right(), literals);
        }
        return;
    }

    if (auto const* logical = dynamic_cast<LogicalExpr const*>(&node)) {
        if (logical->left()) {
            visit(*logical->left(), literals);
        }
        if (logical->right()) {
            visit(*logical->right(), literals);
        }
        return;
    }

    if (auto const* notExpr = dynamic_cast<NotExpr const*>(&node)) {
        if (notExpr->child()) {
            visit(*notExpr->child(), literals);
        }
        return;
    }
}

std::vector<ExtractedLiteral> LiteralExtractor::extract(ASTNode const& root) const
{
    std::vector<ExtractedLiteral> literals;
    visit(root, literals);
    return literals;
}

}
