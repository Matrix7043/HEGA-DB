#include "hegadb/client/preparation/LiteralExtractor.h"
#include "hegadb/client/ast/ASTNode.h"
#include "hegadb/client/ast/ComparisonExpr.h"
#include "hegadb/client/ast/LogicalExpr.h"
#include "hegadb/client/ast/NotExpr.h"
#include "hegadb/client/ast/SelectStmt.h"
#include <vector>

namespace hegadb::client::preparation {

void LiteralExtractor::visit(
    ast::ASTNode const& node,
    std::vector<ExtractedLiteral>& literals) const
{
    if (auto const* selectStmt = dynamic_cast<ast::SelectStmt const*>(&node)) {
        if (selectStmt->where()) {
            visit(*selectStmt->where(), literals);
        }
        return;
    }

    if (auto const* literal = dynamic_cast<ast::Literal const*>(&node)) {

        literals.push_back({ literals.size(), literal->value() });
        return;
    }

    if (auto const* comparison = dynamic_cast<ast::ComparisonExpr const*>(&node)) {
        if (comparison->left()) {
            visit(*comparison->left(), literals);
        }
        if (comparison->right()) {
            visit(*comparison->right(), literals);
        }
        return;
    }

    if (auto const* logical = dynamic_cast<ast::LogicalExpr const*>(&node)) {
        if (logical->left()) {
            visit(*logical->left(), literals);
        }
        if (logical->right()) {
            visit(*logical->right(), literals);
        }
        return;
    }

    if (auto const* notExpr = dynamic_cast<ast::NotExpr const*>(&node)) {
        if (notExpr->child()) {
            visit(*notExpr->child(), literals);
        }
        return;
    }
}

std::vector<ExtractedLiteral> LiteralExtractor::extract(ast::ASTNode const& root) const
{
    std::vector<ExtractedLiteral> literals;
    visit(root, literals);
    return literals;
}

}
