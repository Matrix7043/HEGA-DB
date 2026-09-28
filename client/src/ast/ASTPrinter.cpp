#include "hegadb/client/ast/ASTPrinter.h"

#include "hegadb/client/ast/ColumnRef.h"
#include "hegadb/client/ast/ComparisonExpr.h"
#include "hegadb/client/ast/Literal.h"
#include "hegadb/client/ast/LogicalExpr.h"
#include "hegadb/client/ast/NotExpr.h"
#include "hegadb/client/ast/ParameterRef.h"
#include "hegadb/client/ast/SelectStmt.h"
#include "hegadb/client/ast/TableRef.h"

#include <filesystem>
#include <fstream>
#include <ostream>

namespace hegadb::client::ast {

std::filesystem::path ASTPrinter::print(
    SelectStmt const* stmt,
    std::filesystem::path const& directory,
    std::string const& queryId) const
{
    if (stmt == nullptr) {
        return { };
    }

    std::filesystem::create_directories(directory);

    auto path = directory / (queryId + ".ast");

    std::ofstream output(path);

    if (!output) {
        return { };
    }

    print(stmt, output);

    return path;
}

void ASTPrinter::print(
    SelectStmt const* stmt,
    std::ostream& output) const
{
    if (stmt == nullptr) {
        return;
    }

    output << "SelectStmt\n";

    output << "├── Projections\n";

    auto const& projections = stmt->projections();

    for (std::size_t i = 0; i < projections.size(); ++i) {
        bool const isLast = (i == projections.size() - 1);

        printExpr(
            projections[i].get(),
            output,
            "│   ",
            isLast);
    }

    output << "├── From\n";

    if (stmt->from() != nullptr) {
        output
            << "│   └── TableRef: "
            << stmt->from()->name()
            << '\n';
    }

    output << "└── Where\n";

    if (stmt->where() != nullptr) {
        printExpr(
            stmt->where(),
            output,
            "    ",
            true);
    } else {
        output << "    └── <none>\n";
    }
}

void ASTPrinter::printExpr(
    Expr const* expr,
    std::ostream& output,
    std::string const& prefix,
    bool isLast) const
{
    if (expr == nullptr) {
        return;
    }

    char const* branch = isLast ? "└── " : "├── ";

    switch (expr->kind()) {

    case ASTNodeKind::ColumnRef: {
        auto* column = static_cast<ColumnRef const*>(expr);

        output
            << prefix
            << branch
            << "ColumnRef: "
            << column->name()
            << '\n';

        break;
    }

    case ASTNodeKind::Wildcard: {
        output
            << prefix
            << branch
            << "Wildcard\n";

        break;
    }

    case ASTNodeKind::Literal: {
        /*
         * Literals should not appear in the
         * parameterized AST.
         *
         * Do not print their value because the
         * debug artifact must not expose plaintext.
         */
        output
            << prefix
            << branch
            << "Literal: <redacted>\n";

        break;
    }

    case ASTNodeKind::ComparisonExpr: {
        auto* comparison = static_cast<ComparisonExpr const*>(expr);

        output
            << prefix
            << branch
            << "ComparisonExpr: ";

        switch (comparison->op()) {

        case operations::ComparisonOp::EQ:
            output << '=';
            break;

        case operations::ComparisonOp::NEQ:
            output << "!=";
            break;

        case operations::ComparisonOp::LT:
            output << '<';
            break;

        case operations::ComparisonOp::GT:
            output << '>';
            break;

        case operations::ComparisonOp::LTE:
            output << "<=";
            break;

        case operations::ComparisonOp::GTE:
            output << ">=";
            break;
        }

        output << '\n';

        printExpr(
            comparison->left(),
            output,
            prefix + (isLast ? "    " : "│   "),
            false);

        printExpr(
            comparison->right(),
            output,
            prefix + (isLast ? "    " : "│   "),
            true);

        break;
    }

    case ASTNodeKind::LogicalExpr: {
        auto* logical = static_cast<LogicalExpr const*>(expr);

        output
            << prefix
            << branch
            << "LogicalExpr: ";

        switch (logical->op()) {

        case operations::LogicalOp::AND:
            output << "AND";
            break;

        case operations::LogicalOp::OR:
            output << "OR";
            break;
        }

        output << '\n';

        printExpr(
            logical->left(),
            output,
            prefix + (isLast ? "    " : "│   "),
            false);

        printExpr(
            logical->right(),
            output,
            prefix + (isLast ? "    " : "│   "),
            true);

        break;
    }

    case ASTNodeKind::NotExpr: {
        auto* notExpr = static_cast<NotExpr const*>(expr);

        output
            << prefix
            << branch
            << "NotExpr\n";

        printExpr(
            notExpr->child(),
            output,
            prefix + (isLast ? "    " : "│   "),
            true);

        break;
    }

    case ASTNodeKind::ParameterRef: {
        auto* parameter = static_cast<ParameterRef const*>(expr);

        output
            << prefix
            << branch
            << "ParameterRef: "
            << parameter->parameterIndex()
            << '\n';

        break;
    }

    default:
        output
            << prefix
            << branch
            << "Unknown expression\n";

        break;
    }
}

}
