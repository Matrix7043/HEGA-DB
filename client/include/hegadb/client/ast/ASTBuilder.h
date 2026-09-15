#ifndef AST_BUILDER_H
#define AST_BUILDER_H

#include "HEGADBParser.h"

#include "HEGADBParserBaseVisitor.h"

namespace hegadb::client::ast {

class ASTBuilder : public HEGADBParserBaseVisitor {
public:
    std::any visitQuery(HEGADBParser::QueryContext* ctx) override;

    std::any
    visitSelectStatement(HEGADBParser::SelectStatementContext* ctx) override;

    std::any visitTableName(HEGADBParser::TableNameContext* ctx) override;

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
