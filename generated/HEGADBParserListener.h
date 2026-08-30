
// Generated from ./HEGADBParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "HEGADBParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by HEGADBParser.
 */
class  HEGADBParserListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterQuery(HEGADBParser::QueryContext *ctx) = 0;
  virtual void exitQuery(HEGADBParser::QueryContext *ctx) = 0;

  virtual void enterSelectStatement(HEGADBParser::SelectStatementContext *ctx) = 0;
  virtual void exitSelectStatement(HEGADBParser::SelectStatementContext *ctx) = 0;

  virtual void enterSelectList(HEGADBParser::SelectListContext *ctx) = 0;
  virtual void exitSelectList(HEGADBParser::SelectListContext *ctx) = 0;

  virtual void enterTableName(HEGADBParser::TableNameContext *ctx) = 0;
  virtual void exitTableName(HEGADBParser::TableNameContext *ctx) = 0;

  virtual void enterWhereClause(HEGADBParser::WhereClauseContext *ctx) = 0;
  virtual void exitWhereClause(HEGADBParser::WhereClauseContext *ctx) = 0;

  virtual void enterBooleanExpression(HEGADBParser::BooleanExpressionContext *ctx) = 0;
  virtual void exitBooleanExpression(HEGADBParser::BooleanExpressionContext *ctx) = 0;

  virtual void enterOrExpression(HEGADBParser::OrExpressionContext *ctx) = 0;
  virtual void exitOrExpression(HEGADBParser::OrExpressionContext *ctx) = 0;

  virtual void enterAndExpression(HEGADBParser::AndExpressionContext *ctx) = 0;
  virtual void exitAndExpression(HEGADBParser::AndExpressionContext *ctx) = 0;

  virtual void enterNotExpression(HEGADBParser::NotExpressionContext *ctx) = 0;
  virtual void exitNotExpression(HEGADBParser::NotExpressionContext *ctx) = 0;

  virtual void enterComparisonExpression(HEGADBParser::ComparisonExpressionContext *ctx) = 0;
  virtual void exitComparisonExpression(HEGADBParser::ComparisonExpressionContext *ctx) = 0;

  virtual void enterComparisonOperator(HEGADBParser::ComparisonOperatorContext *ctx) = 0;
  virtual void exitComparisonOperator(HEGADBParser::ComparisonOperatorContext *ctx) = 0;

  virtual void enterOperand(HEGADBParser::OperandContext *ctx) = 0;
  virtual void exitOperand(HEGADBParser::OperandContext *ctx) = 0;

  virtual void enterColumnName(HEGADBParser::ColumnNameContext *ctx) = 0;
  virtual void exitColumnName(HEGADBParser::ColumnNameContext *ctx) = 0;

  virtual void enterLiteral(HEGADBParser::LiteralContext *ctx) = 0;
  virtual void exitLiteral(HEGADBParser::LiteralContext *ctx) = 0;


};

