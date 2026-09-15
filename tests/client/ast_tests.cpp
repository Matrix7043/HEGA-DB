#include <hegadb/client/ast/ColumnRef.h>
#include <hegadb/client/ast/ComparisonExpr.h>
#include <hegadb/client/ast/Literal.h>
#include <hegadb/client/ast/LogicalExpr.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/ast/TableRef.h>

#include <hegadb/operations/ComparisonOp.h>
#include <hegadb/operations/LogicalOp.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

int main()
{
    auto query = std::make_unique<hegadb::client::ast::SelectStmt>();

    query->addProjection(
        std::make_unique<hegadb::client::ast::ColumnRef>("salary"));

    query->addProjection(
        std::make_unique<hegadb::client::ast::ColumnRef>("name"));

    query->setFrom(
        std::make_unique<hegadb::client::ast::TableRef>("employees"));

    auto salaryComparison = std::make_unique<hegadb::client::ast::ComparisonExpr>(
        std::make_unique<hegadb::client::ast::ColumnRef>("salary"),
        hegadb::operations::ComparisonOp::GT,
        std::make_unique<hegadb::client::ast::Literal>(50000LL));

    auto nameComparison = std::make_unique<hegadb::client::ast::ComparisonExpr>(
        std::make_unique<hegadb::client::ast::ColumnRef>("name"),
        hegadb::operations::ComparisonOp::EQ,
        std::make_unique<hegadb::client::ast::Literal>(
            std::string("Bob")));

    auto where = std::make_unique<hegadb::client::ast::LogicalExpr>(
        std::move(salaryComparison),
        hegadb::operations::LogicalOp::AND,
        std::move(nameComparison));

    query->setWhere(std::move(where));

    assert(query->projections().size() == 2);
    assert(query->from() != nullptr);
    assert(query->from()->name() == "employees");
    assert(query->where() != nullptr);

    std::cout << "AST test passed!\n";

    return 0;
}
