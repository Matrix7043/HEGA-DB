#include "hegadb/client/ast/ASTPrinter.h"

#include "hegadb/client/ast/ColumnRef.h"
#include "hegadb/client/ast/ComparisonExpr.h"
#include "hegadb/client/ast/Literal.h"
#include "hegadb/client/ast/LogicalExpr.h"
#include "hegadb/client/ast/NotExpr.h"
#include "hegadb/client/ast/ParameterRef.h"
#include "hegadb/client/ast/TableRef.h"

#include <iostream>
#include <variant>

namespace hegadb::client::ast {

void ASTPrinter::print(SelectStmt const* stmt) const
{
    if (stmt == nullptr) {
        return;
    }

    std::cout << "SelectStmt\n";

    std::cout << "├── Projections\n";

    auto const& projections = stmt->projections();

    for (std::size_t i = 0; i < projections.size(); ++i) {
        bool const isLast = (i == projections.size() - 1);

        printExpr(projections[i].get(), "│   ", isLast);
    }

    std::cout << "├── From\n";

    if (stmt->from() != nullptr) {
        std::cout << "│   └── TableRef: " << stmt->from()->name() << '\n';
    }

    std::cout << "└── Where\n";

    if (stmt->where() != nullptr) {
        printExpr(stmt->where(), "    ", true);
    } else {
        std::cout << "    └── <none>\n";
    }
}

void ASTPrinter::printExpr(Expr const* expr, std::string const& prefix,
    bool isLast) const
{

    if (expr == nullptr) {
        return;
    }

    char const* branch = isLast ? "└── " : "├── ";

    switch (expr->kind()) {

    case ASTNodeKind::ColumnRef: {
        auto* column = static_cast<ColumnRef const*>(expr);

        std::cout << prefix << branch << "ColumnRef: " << column->name() << '\n';

        break;
    }

    case ASTNodeKind::Wildcard: {
        std::cout << prefix << branch << "Wildcard\n";

        break;
    }

    case ASTNodeKind::Literal: {
        auto* literal = static_cast<Literal const*>(expr);

        std::cout << prefix << branch << "Literal: ";

        std::visit([](auto const& value) { std::cout << value; }, literal->value());

        std::cout << '\n';

        break;
    }

    case ASTNodeKind::ComparisonExpr: {
        auto* comparison = static_cast<ComparisonExpr const*>(expr);

        std::cout << prefix << branch << "ComparisonExpr: ";

        switch (comparison->op()) {
        case operations::ComparisonOp::EQ:
            std::cout << '=';
            break;
        case operations::ComparisonOp::NEQ:
            std::cout << "!=";
            break;
        case operations::ComparisonOp::LT:
            std::cout << '<';
            break;
        case operations::ComparisonOp::GT:
            std::cout << '>';
            break;
        case operations::ComparisonOp::LTE:
            std::cout << "<=";
            break;
        case operations::ComparisonOp::GTE:
            std::cout << ">=";
            break;
        }

        std::cout << '\n';

        printExpr(comparison->left(), prefix + (isLast ? "    " : "│   "), false);

        printExpr(comparison->right(), prefix + (isLast ? "    " : "│   "), true);

        break;
    }

    case ASTNodeKind::LogicalExpr: {
        auto* logical = static_cast<LogicalExpr const*>(expr);

        std::cout << prefix << branch << "LogicalExpr: ";

        switch (logical->op()) {
        case operations::LogicalOp::AND:
            std::cout << "AND";
            break;
        case operations::LogicalOp::OR:
            std::cout << "OR";
            break;
        }

        std::cout << '\n';

        printExpr(logical->left(), prefix + (isLast ? "    " : "│   "), false);

        printExpr(logical->right(), prefix + (isLast ? "    " : "│   "), true);

        break;
    }

    case ASTNodeKind::NotExpr: {
        auto* notExpr = static_cast<NotExpr const*>(expr);

        std::cout << prefix << branch << "NotExpr\n";

        printExpr(notExpr->child(), prefix + (isLast ? "    " : "│   "), true);

        break;
    }

    case ASTNodeKind::ParameterRef: {
        auto* parameter = static_cast<ParameterRef const*>(expr);

        std::cout << prefix << branch
                  << "ParameterRef: "
                  << parameter->parameterIndex()
                  << '\n';

        break;
    }

    default:
        std::cout << prefix << branch << "Unknown expression\n";
        break;
    }
}

} // namespace hegadb
