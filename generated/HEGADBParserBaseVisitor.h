
// Generated from ./HEGADBParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "HEGADBParserVisitor.h"


/**
 * This class provides an empty implementation of HEGADBParserVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  HEGADBParserBaseVisitor : public HEGADBParserVisitor {
public:

  virtual std::any visitQuery(HEGADBParser::QueryContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelectStatement(HEGADBParser::SelectStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelectList(HEGADBParser::SelectListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTableName(HEGADBParser::TableNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWhereClause(HEGADBParser::WhereClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBooleanExpression(HEGADBParser::BooleanExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOrExpression(HEGADBParser::OrExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAndExpression(HEGADBParser::AndExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNotExpression(HEGADBParser::NotExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitComparisonExpression(HEGADBParser::ComparisonExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitComparisonOperator(HEGADBParser::ComparisonOperatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperand(HEGADBParser::OperandContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitColumnName(HEGADBParser::ColumnNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLiteral(HEGADBParser::LiteralContext *ctx) override {
    return visitChildren(ctx);
  }


};

