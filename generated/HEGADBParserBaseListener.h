
// Generated from ./HEGADBParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "HEGADBParserListener.h"


/**
 * This class provides an empty implementation of HEGADBParserListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  HEGADBParserBaseListener : public HEGADBParserListener {
public:

  virtual void enterQuery(HEGADBParser::QueryContext * /*ctx*/) override { }
  virtual void exitQuery(HEGADBParser::QueryContext * /*ctx*/) override { }

  virtual void enterSelectStatement(HEGADBParser::SelectStatementContext * /*ctx*/) override { }
  virtual void exitSelectStatement(HEGADBParser::SelectStatementContext * /*ctx*/) override { }

  virtual void enterSelectList(HEGADBParser::SelectListContext * /*ctx*/) override { }
  virtual void exitSelectList(HEGADBParser::SelectListContext * /*ctx*/) override { }

  virtual void enterTableName(HEGADBParser::TableNameContext * /*ctx*/) override { }
  virtual void exitTableName(HEGADBParser::TableNameContext * /*ctx*/) override { }

  virtual void enterWhereClause(HEGADBParser::WhereClauseContext * /*ctx*/) override { }
  virtual void exitWhereClause(HEGADBParser::WhereClauseContext * /*ctx*/) override { }

  virtual void enterBooleanExpression(HEGADBParser::BooleanExpressionContext * /*ctx*/) override { }
  virtual void exitBooleanExpression(HEGADBParser::BooleanExpressionContext * /*ctx*/) override { }

  virtual void enterOrExpression(HEGADBParser::OrExpressionContext * /*ctx*/) override { }
  virtual void exitOrExpression(HEGADBParser::OrExpressionContext * /*ctx*/) override { }

  virtual void enterAndExpression(HEGADBParser::AndExpressionContext * /*ctx*/) override { }
  virtual void exitAndExpression(HEGADBParser::AndExpressionContext * /*ctx*/) override { }

  virtual void enterNotExpression(HEGADBParser::NotExpressionContext * /*ctx*/) override { }
  virtual void exitNotExpression(HEGADBParser::NotExpressionContext * /*ctx*/) override { }

  virtual void enterComparisonExpression(HEGADBParser::ComparisonExpressionContext * /*ctx*/) override { }
  virtual void exitComparisonExpression(HEGADBParser::ComparisonExpressionContext * /*ctx*/) override { }

  virtual void enterComparisonOperator(HEGADBParser::ComparisonOperatorContext * /*ctx*/) override { }
  virtual void exitComparisonOperator(HEGADBParser::ComparisonOperatorContext * /*ctx*/) override { }

  virtual void enterOperand(HEGADBParser::OperandContext * /*ctx*/) override { }
  virtual void exitOperand(HEGADBParser::OperandContext * /*ctx*/) override { }

  virtual void enterColumnName(HEGADBParser::ColumnNameContext * /*ctx*/) override { }
  virtual void exitColumnName(HEGADBParser::ColumnNameContext * /*ctx*/) override { }

  virtual void enterLiteral(HEGADBParser::LiteralContext * /*ctx*/) override { }
  virtual void exitLiteral(HEGADBParser::LiteralContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

