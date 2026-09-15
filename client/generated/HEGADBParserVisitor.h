
// Generated from ./HEGADBParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "HEGADBParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by HEGADBParser.
 */
class  HEGADBParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by HEGADBParser.
   */
    virtual std::any visitQuery(HEGADBParser::QueryContext *context) = 0;

    virtual std::any visitSelectStatement(HEGADBParser::SelectStatementContext *context) = 0;

    virtual std::any visitSelectList(HEGADBParser::SelectListContext *context) = 0;

    virtual std::any visitTableName(HEGADBParser::TableNameContext *context) = 0;

    virtual std::any visitWhereClause(HEGADBParser::WhereClauseContext *context) = 0;

    virtual std::any visitBooleanExpression(HEGADBParser::BooleanExpressionContext *context) = 0;

    virtual std::any visitOrExpression(HEGADBParser::OrExpressionContext *context) = 0;

    virtual std::any visitAndExpression(HEGADBParser::AndExpressionContext *context) = 0;

    virtual std::any visitNotExpression(HEGADBParser::NotExpressionContext *context) = 0;

    virtual std::any visitComparisonExpression(HEGADBParser::ComparisonExpressionContext *context) = 0;

    virtual std::any visitComparisonOperator(HEGADBParser::ComparisonOperatorContext *context) = 0;

    virtual std::any visitOperand(HEGADBParser::OperandContext *context) = 0;

    virtual std::any visitColumnName(HEGADBParser::ColumnNameContext *context) = 0;

    virtual std::any visitLiteral(HEGADBParser::LiteralContext *context) = 0;


};

