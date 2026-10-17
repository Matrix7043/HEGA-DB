#include "hegadb/client/ast/ASTBuilder.h"
#include "HEGADBParser.h"
#include "hegadb/client/ast/ColumnRef.h"
#include "hegadb/client/ast/ComparisonExpr.h"
#include "hegadb/client/ast/CreateTableStmt.h"
#include "hegadb/client/ast/DropTableStmt.h"
#include "hegadb/client/ast/InsertStmt.h"
#include "hegadb/client/ast/JoinClause.h"
#include "hegadb/client/ast/Literal.h"
#include "hegadb/client/ast/LogicalExpr.h"
#include "hegadb/client/ast/NullLiteral.h"
#include "hegadb/client/ast/NotExpr.h"
#include "hegadb/client/ast/SelectStmt.h"
#include "hegadb/client/ast/Statement.h"
#include "hegadb/client/ast/TableRef.h"
#include "hegadb/client/ast/Wildcard.h"
#include "hegadb/operations/ComparisonOp.h"
#include "hegadb/types/DataType.h"

#include <any>
#include <memory>
#include <stdexcept>
#include <vector>

namespace hegadb::client::ast {

std::any ASTBuilder::visitQuery(HEGADBParser::QueryContext* ctx)
{
    return visit(ctx->statement());
}

std::any ASTBuilder::visitStatement(HEGADBParser::StatementContext* ctx)
{
    if (ctx->selectStatement() != nullptr) {
        return visit(ctx->selectStatement());
    }

    if (ctx->createTableStatement() != nullptr) {
        return visit(ctx->createTableStatement());
    }

    if (ctx->dropTableStatement() != nullptr) {
        return visit(ctx->dropTableStatement());
    }

    if (ctx->insertStatement() != nullptr) {
        return visit(ctx->insertStatement());
    }

    throw std::runtime_error("Unknown statement type");
}

std::any
ASTBuilder::visitSelectStatement(HEGADBParser::SelectStatementContext* ctx)
{
    auto stmt = std::make_unique<SelectStmt>();

    auto* selectList = ctx->selectList();

    if (selectList->STAR() != nullptr) {
        stmt->addProjection(std::make_unique<Wildcard>());
    } else {
        for (auto* item : selectList->selectItem()) {
            if (item->qualifiedColumn() != nullptr) {
                auto result = visit(item->qualifiedColumn());
                std::unique_ptr<Expr> column(std::any_cast<ColumnRef*>(result));
                stmt->addProjection(std::move(column));
            } else {
                auto name = item->columnName()->IDENTIFIER()->getText();
                stmt->addProjection(std::make_unique<ColumnRef>(name));
            }
        }
    }

    auto tableResult = visit(ctx->tableName());

    auto table = std::any_cast<TableRef*>(tableResult);

    stmt->setFrom(std::unique_ptr<TableRef>(table));

    if (ctx->joinClause() != nullptr) {
        auto joinResult = visit(ctx->joinClause());
        stmt->setJoin(std::unique_ptr<JoinClause>(
            std::any_cast<JoinClause*>(joinResult)));
    }

    if (ctx->whereClause() != nullptr) {
        auto whereResult = visit(ctx->whereClause());
        auto* where = std::any_cast<Expr*>(whereResult);
        stmt->setWhere(std::unique_ptr<Expr>(where));
    }

    Statement* result = stmt.release();
    return result;
}

std::any ASTBuilder::visitCreateTableStatement(
    HEGADBParser::CreateTableStatementContext* ctx)
{
    auto stmt = std::make_unique<CreateTableStmt>(
        ctx->tableName()->IDENTIFIER()->getText());

    for (auto* definition : ctx->columnDefinition()) {
        auto result = visit(definition);
        stmt->addColumn(std::any_cast<ColumnDef>(result));
    }

    Statement* result = stmt.release();
    return result;
}

std::any ASTBuilder::visitColumnDefinition(
    HEGADBParser::ColumnDefinitionContext* ctx)
{
    auto name = ctx->columnName()->IDENTIFIER()->getText();

    auto typeResult = visit(ctx->dataType());
    auto type = std::any_cast<types::DataType>(typeResult);

    // Columns are nullable unless NOT NULL is given.
    bool const nullable = (ctx->nullability() == nullptr);

    return ColumnDef { std::move(name), type, nullable };
}

std::any ASTBuilder::visitDataType(HEGADBParser::DataTypeContext* ctx)
{
    if (ctx->INTEGER() != nullptr) {
        return types::DataType::Integer;
    }

    if (ctx->FLOAT() != nullptr) {
        return types::DataType::Float;
    }

    if (ctx->STRING() != nullptr) {
        return types::DataType::String;
    }

    if (ctx->BOOLEAN() != nullptr) {
        return types::DataType::Boolean;
    }

    throw std::runtime_error("Unknown data type");
}

std::any ASTBuilder::visitDropTableStatement(
    HEGADBParser::DropTableStatementContext* ctx)
{
    auto stmt = std::make_unique<DropTableStmt>(
        ctx->tableName()->IDENTIFIER()->getText());

    Statement* result = stmt.release();
    return result;
}

std::any ASTBuilder::visitInsertStatement(
    HEGADBParser::InsertStatementContext* ctx)
{
    auto stmt = std::make_unique<InsertStmt>(
        ctx->tableName()->IDENTIFIER()->getText());

    // The column list is optional; columnName() is empty without it.
    for (auto* column : ctx->columnName()) {
        stmt->addColumn(column->IDENTIFIER()->getText());
    }

    // std::any cannot hold a move-only row, so the rows are built here.
    for (auto* row_ctx : ctx->valueRow()) {
        InsertStmt::Row row;

        for (auto* value : row_ctx->insertValue()) {
            auto result = visit(value);
            row.emplace_back(std::any_cast<Expr*>(result));
        }

        stmt->addRow(std::move(row));
    }

    Statement* result = stmt.release();
    return result;
}

std::any ASTBuilder::visitInsertValue(HEGADBParser::InsertValueContext* ctx)
{
    if (ctx->NULL_() != nullptr) {
        Expr* expr = new NullLiteral();
        return expr;
    }

    return visit(ctx->literal());
}

std::any ASTBuilder::visitTableName(HEGADBParser::TableNameContext* ctx)
{
    auto name = ctx->IDENTIFIER()->getText();
    return new TableRef(name);
}

std::any ASTBuilder::visitQualifiedColumn(
    HEGADBParser::QualifiedColumnContext* ctx)
{
    auto table = ctx->tableName()->IDENTIFIER()->getText();
    auto name = ctx->columnName()->IDENTIFIER()->getText();

    ColumnRef* column = new ColumnRef(std::move(table), std::move(name));
    return column;
}

std::any ASTBuilder::visitJoinClause(HEGADBParser::JoinClauseContext* ctx)
{
    auto tableResult = visit(ctx->tableName());
    std::unique_ptr<TableRef> table(std::any_cast<TableRef*>(tableResult));

    auto leftResult = visit(ctx->qualifiedColumn(0));
    std::unique_ptr<ColumnRef> left(std::any_cast<ColumnRef*>(leftResult));

    auto rightResult = visit(ctx->qualifiedColumn(1));
    std::unique_ptr<ColumnRef> right(std::any_cast<ColumnRef*>(rightResult));

    JoinClause* join = new JoinClause(
        std::move(table), std::move(left), std::move(right));
    return join;
}

std::any ASTBuilder::visitLiteral(HEGADBParser::LiteralContext* ctx)
{
    if (ctx->INTEGER_LITERAL() != nullptr) {
        auto value = std::stoll(ctx->INTEGER_LITERAL()->getText());
        Expr* expr = new Literal(value);
        return expr;
    }

    if (ctx->STRING_LITERAL() != nullptr) {
        auto text = ctx->STRING_LITERAL()->getText();
        text = text.substr(1, text.size() - 2);
        Expr* expr = new Literal(text);
        return expr;
    }

    if (ctx->TRUE() != nullptr) {
        Expr* expr = new Literal(true);
        return expr;
    }

    if (ctx->FALSE() != nullptr) {
        Expr* expr = new Literal(false);
        return expr;
    }

    return nullptr;
}

std::any ASTBuilder::visitOperand(HEGADBParser::OperandContext* ctx)
{
    if (ctx->columnName() != nullptr) {
        auto name = ctx->columnName()->IDENTIFIER()->getText();
        Expr* expr = new ColumnRef(name);
        return expr;
    }

    return visit(ctx->literal());
}

std::any ASTBuilder::visitComparisonOperator(
    HEGADBParser::ComparisonOperatorContext* ctx)
{
    if (ctx->EQ() != nullptr) {
        return operations::ComparisonOp::EQ;
    }

    if (ctx->NEQ() != nullptr) {
        return operations::ComparisonOp::NEQ;
    }

    if (ctx->LT() != nullptr) {
        return operations::ComparisonOp::LT;
    }

    if (ctx->GT() != nullptr) {
        return operations::ComparisonOp::GT;
    }

    if (ctx->LTE() != nullptr) {
        return operations::ComparisonOp::LTE;
    }

    if (ctx->GTE() != nullptr) {
        return operations::ComparisonOp::GTE;
    }

    throw std::runtime_error("Unknown comparison operator");
}

std::any ASTBuilder::visitComparisonExpression(
    HEGADBParser::ComparisonExpressionContext* ctx)
{
    auto leftResult = visit(ctx->operand(0));
    auto rightResult = visit(ctx->operand(1));
    auto opResult = visit(ctx->comparisonOperator());

    auto* left = std::any_cast<Expr*>(leftResult);

    auto* right = std::any_cast<Expr*>(rightResult);

    auto op = std::any_cast<operations::ComparisonOp>(opResult);

    Expr* expr = new ComparisonExpr(std::unique_ptr<Expr>(left), op,
        std::unique_ptr<Expr>(right));
    return expr;
}

std::any
ASTBuilder::visitNotExpression(HEGADBParser::NotExpressionContext* ctx)
{
    if (ctx->NOT() != nullptr) {
        auto result = visit(ctx->notExpression());

        auto* child = std::any_cast<Expr*>(result);

        Expr* expr = new NotExpr(std::unique_ptr<Expr>(child));
        return expr;
    }

    if (ctx->booleanExpression() != nullptr) {
        return visit(ctx->booleanExpression());
    }

    return visit(ctx->comparisonExpression());
}

std::any
ASTBuilder::visitAndExpression(HEGADBParser::AndExpressionContext* ctx)
{
    auto result = visit(ctx->notExpression(0));

    Expr* left = std::any_cast<Expr*>(result);

    for (size_t i = 1; i < ctx->notExpression().size(); ++i) {
        auto rightResult = visit(ctx->notExpression(i));

        auto* right = std::any_cast<Expr*>(rightResult);

        left = new LogicalExpr(std::unique_ptr<Expr>(left), operations::LogicalOp::AND,
            std::unique_ptr<Expr>(right));
    }

    return left;
}

std::any ASTBuilder::visitOrExpression(HEGADBParser::OrExpressionContext* ctx)
{
    auto result = visit(ctx->andExpression(0));

    Expr* left = std::any_cast<Expr*>(result);

    for (size_t i = 1; i < ctx->andExpression().size(); ++i) {
        auto rightResult = visit(ctx->andExpression(i));

        auto* right = std::any_cast<Expr*>(rightResult);

        left = new LogicalExpr(std::unique_ptr<Expr>(left), operations::LogicalOp::OR,
            std::unique_ptr<Expr>(right));
    }

    return left;
}

std::any ASTBuilder::visitBooleanExpression(
    HEGADBParser::BooleanExpressionContext* ctx)
{
    return visit(ctx->orExpression());
}

std::any ASTBuilder::visitWhereClause(HEGADBParser::WhereClauseContext* ctx)
{
    return visit(ctx->booleanExpression());
}

} // namespace hegadb
