#include "ast/ASTPrinter.h"

#include "ast/ColumnRef.h"
#include "ast/ComparisonExpr.h"
#include "ast/Literal.h"
#include "ast/LogicalExpr.h"
#include "ast/NotExpr.h"
#include "ast/TableRef.h"
#include "ast/Wildcard.h"

#include <iostream>
#include <variant>

namespace hegadb {

void ASTPrinter::print(const SelectStmt *stmt) const {
  if (stmt == nullptr) {
    return;
  }

  std::cout << "SelectStmt\n";

  std::cout << "├── Projections\n";

  const auto &projections = stmt->projections();

  for (std::size_t i = 0; i < projections.size(); ++i) {
    const bool isLast = (i == projections.size() - 1);

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

void ASTPrinter::printExpr(const Expr *expr, const std::string &prefix,
                           bool isLast) const {

  if (expr == nullptr) {
    return;
  }

  const char *branch = isLast ? "└── " : "├── ";

  switch (expr->kind()) {

  case ASTNodeKind::ColumnRef: {
    auto *column = static_cast<const ColumnRef *>(expr);

    std::cout << prefix << branch << "ColumnRef: " << column->name() << '\n';

    break;
  }

  case ASTNodeKind::Wildcard: {
    std::cout << prefix << branch << "Wildcard\n";

    break;
  }

  case ASTNodeKind::Literal: {
    auto *literal = static_cast<const Literal *>(expr);

    std::cout << prefix << branch << "Literal: ";

    std::visit([](const auto &value) { std::cout << value; }, literal->value());

    std::cout << '\n';

    break;
  }

  case ASTNodeKind::ComparisonExpr: {
    auto *comparison = static_cast<const ComparisonExpr *>(expr);

    std::cout << prefix << branch << "ComparisonExpr: ";

    switch (comparison->op()) {
    case ComparisonOp::EQ:
      std::cout << '=';
      break;
    case ComparisonOp::NEQ:
      std::cout << "!=";
      break;
    case ComparisonOp::LT:
      std::cout << '<';
      break;
    case ComparisonOp::GT:
      std::cout << '>';
      break;
    case ComparisonOp::LTE:
      std::cout << "<=";
      break;
    case ComparisonOp::GTE:
      std::cout << ">=";
      break;
    }

    std::cout << '\n';

    printExpr(comparison->left(), prefix + (isLast ? "    " : "│   "), false);

    printExpr(comparison->right(), prefix + (isLast ? "    " : "│   "), true);

    break;
  }

  case ASTNodeKind::LogicalExpr: {
    auto *logical = static_cast<const LogicalExpr *>(expr);

    std::cout << prefix << branch << "LogicalExpr: ";

    switch (logical->op()) {
    case LogicalOp::AND:
      std::cout << "AND";
      break;
    case LogicalOp::OR:
      std::cout << "OR";
      break;
    }

    std::cout << '\n';

    printExpr(logical->left(), prefix + (isLast ? "    " : "│   "), false);

    printExpr(logical->right(), prefix + (isLast ? "    " : "│   "), true);

    break;
  }

  case ASTNodeKind::NotExpr: {
    auto *notExpr = static_cast<const NotExpr *>(expr);

    std::cout << prefix << branch << "NotExpr\n";

    printExpr(notExpr->child(), prefix + (isLast ? "    " : "│   "), true);

    break;
  }

  default:
    std::cout << prefix << branch << "Unknown expression\n";
    break;
  }
}

} // namespace hegadb
