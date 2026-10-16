#ifndef AST_BUILDER_H
#define AST_BUILDER_H

#include "HEGADBParser.h"

#include "HEGADBParserBaseVisitor.h"

namespace hegadb::client::ast {

class ASTBuilder : public HEGADBParserBaseVisitor {
public:
    std::any visitQuery(HEGADBParser::QueryContext* ctx) override;

    // Statement visitors (SELECT / CREATE TABLE / DROP TABLE) all return
    // a Statement* wrapped in std::any. The caller owns the result.
    std::any visitStatement(HEGADBParser::StatementContext* ctx) override;

    std::any
    visitSelectStatement(HEGADBParser::SelectStatementContext* ctx) override;

    std::any visitCreateTableStatement(
        HEGADBParser::CreateTableStatementContext* ctx) override;

    // Returns a ColumnDef by value.
    std::any visitColumnDefinition(
        HEGADBParser::ColumnDefinitionContext* ctx) override;

    // Returns a types::DataType by value.
    std::any visitDataType(HEGADBParser::DataTypeContext* ctx) override;

    std::any visitDropTableStatement(
        HEGADBParser::DropTableStatementContext* ctx) override;

    std::any visitTableName(HEGADBParser::TableNameContext* ctx) override;

    // Returns a ColumnRef* (qualified: table.column)
    std::any visitQualifiedColumn(
        HEGADBParser::QualifiedColumnContext* ctx) override;

    // Returns a JoinClause*
    std::any visitJoinClause(HEGADBParser::JoinClauseContext* ctx) override;

    std::any visitLiteral(HEGADBParser::LiteralContext* ctx) override;

    std::any visitOperand(HEGADBParser::OperandContext* ctx) override;

    std::any visitComparisonOperator(
        HEGADBParser::ComparisonOperatorContext* ctx) override;

    std::any visitComparisonExpression(
        HEGADBParser::ComparisonExpressionContext* ctx) override;

    std::any visitNotExpression(HEGADBParser::NotExpressionContext* ctx) override;

    std::any visitAndExpression(HEGADBParser::AndExpressionContext* ctx) override;

    std::any visitOrExpression(HEGADBParser::OrExpressionContext* ctx) override;

    std::any
    visitBooleanExpression(HEGADBParser::BooleanExpressionContext* ctx) override;

    std::any visitWhereClause(HEGADBParser::WhereClauseContext* ctx) override;
};

} // namespace hegadb

#endif
