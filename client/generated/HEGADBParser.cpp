
// Generated from ./HEGADBParser.g4 by ANTLR 4.13.2


#include "HEGADBParserListener.h"
#include "HEGADBParserVisitor.h"

#include "HEGADBParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct HEGADBParserStaticData final {
  HEGADBParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  HEGADBParserStaticData(const HEGADBParserStaticData&) = delete;
  HEGADBParserStaticData(HEGADBParserStaticData&&) = delete;
  HEGADBParserStaticData& operator=(const HEGADBParserStaticData&) = delete;
  HEGADBParserStaticData& operator=(HEGADBParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag hegadbparserParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<HEGADBParserStaticData> hegadbparserParserStaticData = nullptr;

void hegadbparserParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (hegadbparserParserStaticData != nullptr) {
    return;
  }
#else
  assert(hegadbparserParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<HEGADBParserStaticData>(
    std::vector<std::string>{
      "query", "selectStatement", "selectList", "tableName", "whereClause", 
      "booleanExpression", "orExpression", "andExpression", "notExpression", 
      "comparisonExpression", "comparisonOperator", "operand", "columnName", 
      "literal"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "'='", "'!='", "'<='", "'>='", 
      "'<'", "'>'", "'*'", "','", "'('", "')'", "';'"
    },
    std::vector<std::string>{
      "", "SELECT", "FROM", "WHERE", "AND", "OR", "NOT", "TRUE", "FALSE", 
      "EQ", "NEQ", "LTE", "GTE", "LT", "GT", "STAR", "COMMA", "LPAREN", 
      "RPAREN", "SEMICOLON", "INTEGER_LITERAL", "STRING_LITERAL", "IDENTIFIER", 
      "WS", "LINE_COMMENT", "BLOCK_COMMENT"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,25,99,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,7,
  	7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,1,0,1,0,1,
  	0,1,1,1,1,1,1,1,1,1,1,3,1,37,8,1,1,1,3,1,40,8,1,1,2,1,2,1,2,1,2,5,2,46,
  	8,2,10,2,12,2,49,9,2,3,2,51,8,2,1,3,1,3,1,4,1,4,1,4,1,5,1,5,1,6,1,6,1,
  	6,5,6,63,8,6,10,6,12,6,66,9,6,1,7,1,7,1,7,5,7,71,8,7,10,7,12,7,74,9,7,
  	1,8,1,8,1,8,1,8,1,8,1,8,1,8,3,8,83,8,8,1,9,1,9,1,9,1,9,1,10,1,10,1,11,
  	1,11,3,11,93,8,11,1,12,1,12,1,13,1,13,1,13,0,0,14,0,2,4,6,8,10,12,14,
  	16,18,20,22,24,26,0,2,1,0,9,14,2,0,7,8,20,21,93,0,28,1,0,0,0,2,31,1,0,
  	0,0,4,50,1,0,0,0,6,52,1,0,0,0,8,54,1,0,0,0,10,57,1,0,0,0,12,59,1,0,0,
  	0,14,67,1,0,0,0,16,82,1,0,0,0,18,84,1,0,0,0,20,88,1,0,0,0,22,92,1,0,0,
  	0,24,94,1,0,0,0,26,96,1,0,0,0,28,29,3,2,1,0,29,30,5,0,0,1,30,1,1,0,0,
  	0,31,32,5,1,0,0,32,33,3,4,2,0,33,34,5,2,0,0,34,36,3,6,3,0,35,37,3,8,4,
  	0,36,35,1,0,0,0,36,37,1,0,0,0,37,39,1,0,0,0,38,40,5,19,0,0,39,38,1,0,
  	0,0,39,40,1,0,0,0,40,3,1,0,0,0,41,51,5,15,0,0,42,47,3,24,12,0,43,44,5,
  	16,0,0,44,46,3,24,12,0,45,43,1,0,0,0,46,49,1,0,0,0,47,45,1,0,0,0,47,48,
  	1,0,0,0,48,51,1,0,0,0,49,47,1,0,0,0,50,41,1,0,0,0,50,42,1,0,0,0,51,5,
  	1,0,0,0,52,53,5,22,0,0,53,7,1,0,0,0,54,55,5,3,0,0,55,56,3,10,5,0,56,9,
  	1,0,0,0,57,58,3,12,6,0,58,11,1,0,0,0,59,64,3,14,7,0,60,61,5,5,0,0,61,
  	63,3,14,7,0,62,60,1,0,0,0,63,66,1,0,0,0,64,62,1,0,0,0,64,65,1,0,0,0,65,
  	13,1,0,0,0,66,64,1,0,0,0,67,72,3,16,8,0,68,69,5,4,0,0,69,71,3,16,8,0,
  	70,68,1,0,0,0,71,74,1,0,0,0,72,70,1,0,0,0,72,73,1,0,0,0,73,15,1,0,0,0,
  	74,72,1,0,0,0,75,76,5,6,0,0,76,83,3,16,8,0,77,78,5,17,0,0,78,79,3,10,
  	5,0,79,80,5,18,0,0,80,83,1,0,0,0,81,83,3,18,9,0,82,75,1,0,0,0,82,77,1,
  	0,0,0,82,81,1,0,0,0,83,17,1,0,0,0,84,85,3,22,11,0,85,86,3,20,10,0,86,
  	87,3,22,11,0,87,19,1,0,0,0,88,89,7,0,0,0,89,21,1,0,0,0,90,93,3,24,12,
  	0,91,93,3,26,13,0,92,90,1,0,0,0,92,91,1,0,0,0,93,23,1,0,0,0,94,95,5,22,
  	0,0,95,25,1,0,0,0,96,97,7,1,0,0,97,27,1,0,0,0,8,36,39,47,50,64,72,82,
  	92
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  hegadbparserParserStaticData = std::move(staticData);
}

}

HEGADBParser::HEGADBParser(TokenStream *input) : HEGADBParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

HEGADBParser::HEGADBParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  HEGADBParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *hegadbparserParserStaticData->atn, hegadbparserParserStaticData->decisionToDFA, hegadbparserParserStaticData->sharedContextCache, options);
}

HEGADBParser::~HEGADBParser() {
  delete _interpreter;
}

const atn::ATN& HEGADBParser::getATN() const {
  return *hegadbparserParserStaticData->atn;
}

std::string HEGADBParser::getGrammarFileName() const {
  return "HEGADBParser.g4";
}

const std::vector<std::string>& HEGADBParser::getRuleNames() const {
  return hegadbparserParserStaticData->ruleNames;
}

const dfa::Vocabulary& HEGADBParser::getVocabulary() const {
  return hegadbparserParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView HEGADBParser::getSerializedATN() const {
  return hegadbparserParserStaticData->serializedATN;
}


//----------------- QueryContext ------------------------------------------------------------------

HEGADBParser::QueryContext::QueryContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HEGADBParser::SelectStatementContext* HEGADBParser::QueryContext::selectStatement() {
  return getRuleContext<HEGADBParser::SelectStatementContext>(0);
}

tree::TerminalNode* HEGADBParser::QueryContext::EOF() {
  return getToken(HEGADBParser::EOF, 0);
}


size_t HEGADBParser::QueryContext::getRuleIndex() const {
  return HEGADBParser::RuleQuery;
}

void HEGADBParser::QueryContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterQuery(this);
}

void HEGADBParser::QueryContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitQuery(this);
}


std::any HEGADBParser::QueryContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitQuery(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::QueryContext* HEGADBParser::query() {
  QueryContext *_localctx = _tracker.createInstance<QueryContext>(_ctx, getState());
  enterRule(_localctx, 0, HEGADBParser::RuleQuery);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(28);
    selectStatement();
    setState(29);
    match(HEGADBParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SelectStatementContext ------------------------------------------------------------------

HEGADBParser::SelectStatementContext::SelectStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::SelectStatementContext::SELECT() {
  return getToken(HEGADBParser::SELECT, 0);
}

HEGADBParser::SelectListContext* HEGADBParser::SelectStatementContext::selectList() {
  return getRuleContext<HEGADBParser::SelectListContext>(0);
}

tree::TerminalNode* HEGADBParser::SelectStatementContext::FROM() {
  return getToken(HEGADBParser::FROM, 0);
}

HEGADBParser::TableNameContext* HEGADBParser::SelectStatementContext::tableName() {
  return getRuleContext<HEGADBParser::TableNameContext>(0);
}

HEGADBParser::WhereClauseContext* HEGADBParser::SelectStatementContext::whereClause() {
  return getRuleContext<HEGADBParser::WhereClauseContext>(0);
}

tree::TerminalNode* HEGADBParser::SelectStatementContext::SEMICOLON() {
  return getToken(HEGADBParser::SEMICOLON, 0);
}


size_t HEGADBParser::SelectStatementContext::getRuleIndex() const {
  return HEGADBParser::RuleSelectStatement;
}

void HEGADBParser::SelectStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSelectStatement(this);
}

void HEGADBParser::SelectStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSelectStatement(this);
}


std::any HEGADBParser::SelectStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitSelectStatement(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::SelectStatementContext* HEGADBParser::selectStatement() {
  SelectStatementContext *_localctx = _tracker.createInstance<SelectStatementContext>(_ctx, getState());
  enterRule(_localctx, 2, HEGADBParser::RuleSelectStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(31);
    match(HEGADBParser::SELECT);
    setState(32);
    selectList();
    setState(33);
    match(HEGADBParser::FROM);
    setState(34);
    tableName();
    setState(36);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::WHERE) {
      setState(35);
      whereClause();
    }
    setState(39);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::SEMICOLON) {
      setState(38);
      match(HEGADBParser::SEMICOLON);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SelectListContext ------------------------------------------------------------------

HEGADBParser::SelectListContext::SelectListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::SelectListContext::STAR() {
  return getToken(HEGADBParser::STAR, 0);
}

std::vector<HEGADBParser::ColumnNameContext *> HEGADBParser::SelectListContext::columnName() {
  return getRuleContexts<HEGADBParser::ColumnNameContext>();
}

HEGADBParser::ColumnNameContext* HEGADBParser::SelectListContext::columnName(size_t i) {
  return getRuleContext<HEGADBParser::ColumnNameContext>(i);
}

std::vector<tree::TerminalNode *> HEGADBParser::SelectListContext::COMMA() {
  return getTokens(HEGADBParser::COMMA);
}

tree::TerminalNode* HEGADBParser::SelectListContext::COMMA(size_t i) {
  return getToken(HEGADBParser::COMMA, i);
}


size_t HEGADBParser::SelectListContext::getRuleIndex() const {
  return HEGADBParser::RuleSelectList;
}

void HEGADBParser::SelectListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSelectList(this);
}

void HEGADBParser::SelectListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSelectList(this);
}


std::any HEGADBParser::SelectListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitSelectList(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::SelectListContext* HEGADBParser::selectList() {
  SelectListContext *_localctx = _tracker.createInstance<SelectListContext>(_ctx, getState());
  enterRule(_localctx, 4, HEGADBParser::RuleSelectList);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(50);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::STAR: {
        enterOuterAlt(_localctx, 1);
        setState(41);
        match(HEGADBParser::STAR);
        break;
      }

      case HEGADBParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 2);
        setState(42);
        columnName();
        setState(47);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == HEGADBParser::COMMA) {
          setState(43);
          match(HEGADBParser::COMMA);
          setState(44);
          columnName();
          setState(49);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TableNameContext ------------------------------------------------------------------

HEGADBParser::TableNameContext::TableNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::TableNameContext::IDENTIFIER() {
  return getToken(HEGADBParser::IDENTIFIER, 0);
}


size_t HEGADBParser::TableNameContext::getRuleIndex() const {
  return HEGADBParser::RuleTableName;
}

void HEGADBParser::TableNameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTableName(this);
}

void HEGADBParser::TableNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTableName(this);
}


std::any HEGADBParser::TableNameContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitTableName(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::TableNameContext* HEGADBParser::tableName() {
  TableNameContext *_localctx = _tracker.createInstance<TableNameContext>(_ctx, getState());
  enterRule(_localctx, 6, HEGADBParser::RuleTableName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(52);
    match(HEGADBParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- WhereClauseContext ------------------------------------------------------------------

HEGADBParser::WhereClauseContext::WhereClauseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::WhereClauseContext::WHERE() {
  return getToken(HEGADBParser::WHERE, 0);
}

HEGADBParser::BooleanExpressionContext* HEGADBParser::WhereClauseContext::booleanExpression() {
  return getRuleContext<HEGADBParser::BooleanExpressionContext>(0);
}


size_t HEGADBParser::WhereClauseContext::getRuleIndex() const {
  return HEGADBParser::RuleWhereClause;
}

void HEGADBParser::WhereClauseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWhereClause(this);
}

void HEGADBParser::WhereClauseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWhereClause(this);
}


std::any HEGADBParser::WhereClauseContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitWhereClause(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::WhereClauseContext* HEGADBParser::whereClause() {
  WhereClauseContext *_localctx = _tracker.createInstance<WhereClauseContext>(_ctx, getState());
  enterRule(_localctx, 8, HEGADBParser::RuleWhereClause);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(54);
    match(HEGADBParser::WHERE);
    setState(55);
    booleanExpression();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BooleanExpressionContext ------------------------------------------------------------------

HEGADBParser::BooleanExpressionContext::BooleanExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HEGADBParser::OrExpressionContext* HEGADBParser::BooleanExpressionContext::orExpression() {
  return getRuleContext<HEGADBParser::OrExpressionContext>(0);
}


size_t HEGADBParser::BooleanExpressionContext::getRuleIndex() const {
  return HEGADBParser::RuleBooleanExpression;
}

void HEGADBParser::BooleanExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBooleanExpression(this);
}

void HEGADBParser::BooleanExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBooleanExpression(this);
}


std::any HEGADBParser::BooleanExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitBooleanExpression(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::BooleanExpressionContext* HEGADBParser::booleanExpression() {
  BooleanExpressionContext *_localctx = _tracker.createInstance<BooleanExpressionContext>(_ctx, getState());
  enterRule(_localctx, 10, HEGADBParser::RuleBooleanExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(57);
    orExpression();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OrExpressionContext ------------------------------------------------------------------

HEGADBParser::OrExpressionContext::OrExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<HEGADBParser::AndExpressionContext *> HEGADBParser::OrExpressionContext::andExpression() {
  return getRuleContexts<HEGADBParser::AndExpressionContext>();
}

HEGADBParser::AndExpressionContext* HEGADBParser::OrExpressionContext::andExpression(size_t i) {
  return getRuleContext<HEGADBParser::AndExpressionContext>(i);
}

std::vector<tree::TerminalNode *> HEGADBParser::OrExpressionContext::OR() {
  return getTokens(HEGADBParser::OR);
}

tree::TerminalNode* HEGADBParser::OrExpressionContext::OR(size_t i) {
  return getToken(HEGADBParser::OR, i);
}


size_t HEGADBParser::OrExpressionContext::getRuleIndex() const {
  return HEGADBParser::RuleOrExpression;
}

void HEGADBParser::OrExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOrExpression(this);
}

void HEGADBParser::OrExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOrExpression(this);
}


std::any HEGADBParser::OrExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitOrExpression(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::OrExpressionContext* HEGADBParser::orExpression() {
  OrExpressionContext *_localctx = _tracker.createInstance<OrExpressionContext>(_ctx, getState());
  enterRule(_localctx, 12, HEGADBParser::RuleOrExpression);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(59);
    andExpression();
    setState(64);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HEGADBParser::OR) {
      setState(60);
      match(HEGADBParser::OR);
      setState(61);
      andExpression();
      setState(66);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AndExpressionContext ------------------------------------------------------------------

HEGADBParser::AndExpressionContext::AndExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<HEGADBParser::NotExpressionContext *> HEGADBParser::AndExpressionContext::notExpression() {
  return getRuleContexts<HEGADBParser::NotExpressionContext>();
}

HEGADBParser::NotExpressionContext* HEGADBParser::AndExpressionContext::notExpression(size_t i) {
  return getRuleContext<HEGADBParser::NotExpressionContext>(i);
}

std::vector<tree::TerminalNode *> HEGADBParser::AndExpressionContext::AND() {
  return getTokens(HEGADBParser::AND);
}

tree::TerminalNode* HEGADBParser::AndExpressionContext::AND(size_t i) {
  return getToken(HEGADBParser::AND, i);
}


size_t HEGADBParser::AndExpressionContext::getRuleIndex() const {
  return HEGADBParser::RuleAndExpression;
}

void HEGADBParser::AndExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAndExpression(this);
}

void HEGADBParser::AndExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAndExpression(this);
}


std::any HEGADBParser::AndExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitAndExpression(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::AndExpressionContext* HEGADBParser::andExpression() {
  AndExpressionContext *_localctx = _tracker.createInstance<AndExpressionContext>(_ctx, getState());
  enterRule(_localctx, 14, HEGADBParser::RuleAndExpression);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(67);
    notExpression();
    setState(72);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HEGADBParser::AND) {
      setState(68);
      match(HEGADBParser::AND);
      setState(69);
      notExpression();
      setState(74);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NotExpressionContext ------------------------------------------------------------------

HEGADBParser::NotExpressionContext::NotExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::NotExpressionContext::NOT() {
  return getToken(HEGADBParser::NOT, 0);
}

HEGADBParser::NotExpressionContext* HEGADBParser::NotExpressionContext::notExpression() {
  return getRuleContext<HEGADBParser::NotExpressionContext>(0);
}

tree::TerminalNode* HEGADBParser::NotExpressionContext::LPAREN() {
  return getToken(HEGADBParser::LPAREN, 0);
}

HEGADBParser::BooleanExpressionContext* HEGADBParser::NotExpressionContext::booleanExpression() {
  return getRuleContext<HEGADBParser::BooleanExpressionContext>(0);
}

tree::TerminalNode* HEGADBParser::NotExpressionContext::RPAREN() {
  return getToken(HEGADBParser::RPAREN, 0);
}

HEGADBParser::ComparisonExpressionContext* HEGADBParser::NotExpressionContext::comparisonExpression() {
  return getRuleContext<HEGADBParser::ComparisonExpressionContext>(0);
}


size_t HEGADBParser::NotExpressionContext::getRuleIndex() const {
  return HEGADBParser::RuleNotExpression;
}

void HEGADBParser::NotExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNotExpression(this);
}

void HEGADBParser::NotExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNotExpression(this);
}


std::any HEGADBParser::NotExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitNotExpression(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::NotExpressionContext* HEGADBParser::notExpression() {
  NotExpressionContext *_localctx = _tracker.createInstance<NotExpressionContext>(_ctx, getState());
  enterRule(_localctx, 16, HEGADBParser::RuleNotExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(82);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::NOT: {
        enterOuterAlt(_localctx, 1);
        setState(75);
        match(HEGADBParser::NOT);
        setState(76);
        notExpression();
        break;
      }

      case HEGADBParser::LPAREN: {
        enterOuterAlt(_localctx, 2);
        setState(77);
        match(HEGADBParser::LPAREN);
        setState(78);
        booleanExpression();
        setState(79);
        match(HEGADBParser::RPAREN);
        break;
      }

      case HEGADBParser::TRUE:
      case HEGADBParser::FALSE:
      case HEGADBParser::INTEGER_LITERAL:
      case HEGADBParser::STRING_LITERAL:
      case HEGADBParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 3);
        setState(81);
        comparisonExpression();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ComparisonExpressionContext ------------------------------------------------------------------

HEGADBParser::ComparisonExpressionContext::ComparisonExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<HEGADBParser::OperandContext *> HEGADBParser::ComparisonExpressionContext::operand() {
  return getRuleContexts<HEGADBParser::OperandContext>();
}

HEGADBParser::OperandContext* HEGADBParser::ComparisonExpressionContext::operand(size_t i) {
  return getRuleContext<HEGADBParser::OperandContext>(i);
}

HEGADBParser::ComparisonOperatorContext* HEGADBParser::ComparisonExpressionContext::comparisonOperator() {
  return getRuleContext<HEGADBParser::ComparisonOperatorContext>(0);
}


size_t HEGADBParser::ComparisonExpressionContext::getRuleIndex() const {
  return HEGADBParser::RuleComparisonExpression;
}

void HEGADBParser::ComparisonExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComparisonExpression(this);
}

void HEGADBParser::ComparisonExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComparisonExpression(this);
}


std::any HEGADBParser::ComparisonExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitComparisonExpression(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::ComparisonExpressionContext* HEGADBParser::comparisonExpression() {
  ComparisonExpressionContext *_localctx = _tracker.createInstance<ComparisonExpressionContext>(_ctx, getState());
  enterRule(_localctx, 18, HEGADBParser::RuleComparisonExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(84);
    operand();
    setState(85);
    comparisonOperator();
    setState(86);
    operand();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ComparisonOperatorContext ------------------------------------------------------------------

HEGADBParser::ComparisonOperatorContext::ComparisonOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::ComparisonOperatorContext::EQ() {
  return getToken(HEGADBParser::EQ, 0);
}

tree::TerminalNode* HEGADBParser::ComparisonOperatorContext::NEQ() {
  return getToken(HEGADBParser::NEQ, 0);
}

tree::TerminalNode* HEGADBParser::ComparisonOperatorContext::LT() {
  return getToken(HEGADBParser::LT, 0);
}

tree::TerminalNode* HEGADBParser::ComparisonOperatorContext::GT() {
  return getToken(HEGADBParser::GT, 0);
}

tree::TerminalNode* HEGADBParser::ComparisonOperatorContext::LTE() {
  return getToken(HEGADBParser::LTE, 0);
}

tree::TerminalNode* HEGADBParser::ComparisonOperatorContext::GTE() {
  return getToken(HEGADBParser::GTE, 0);
}


size_t HEGADBParser::ComparisonOperatorContext::getRuleIndex() const {
  return HEGADBParser::RuleComparisonOperator;
}

void HEGADBParser::ComparisonOperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComparisonOperator(this);
}

void HEGADBParser::ComparisonOperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComparisonOperator(this);
}


std::any HEGADBParser::ComparisonOperatorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitComparisonOperator(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::ComparisonOperatorContext* HEGADBParser::comparisonOperator() {
  ComparisonOperatorContext *_localctx = _tracker.createInstance<ComparisonOperatorContext>(_ctx, getState());
  enterRule(_localctx, 20, HEGADBParser::RuleComparisonOperator);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(88);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 32256) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OperandContext ------------------------------------------------------------------

HEGADBParser::OperandContext::OperandContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HEGADBParser::ColumnNameContext* HEGADBParser::OperandContext::columnName() {
  return getRuleContext<HEGADBParser::ColumnNameContext>(0);
}

HEGADBParser::LiteralContext* HEGADBParser::OperandContext::literal() {
  return getRuleContext<HEGADBParser::LiteralContext>(0);
}


size_t HEGADBParser::OperandContext::getRuleIndex() const {
  return HEGADBParser::RuleOperand;
}

void HEGADBParser::OperandContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOperand(this);
}

void HEGADBParser::OperandContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOperand(this);
}


std::any HEGADBParser::OperandContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitOperand(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::OperandContext* HEGADBParser::operand() {
  OperandContext *_localctx = _tracker.createInstance<OperandContext>(_ctx, getState());
  enterRule(_localctx, 22, HEGADBParser::RuleOperand);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(92);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(90);
        columnName();
        break;
      }

      case HEGADBParser::TRUE:
      case HEGADBParser::FALSE:
      case HEGADBParser::INTEGER_LITERAL:
      case HEGADBParser::STRING_LITERAL: {
        enterOuterAlt(_localctx, 2);
        setState(91);
        literal();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ColumnNameContext ------------------------------------------------------------------

HEGADBParser::ColumnNameContext::ColumnNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::ColumnNameContext::IDENTIFIER() {
  return getToken(HEGADBParser::IDENTIFIER, 0);
}


size_t HEGADBParser::ColumnNameContext::getRuleIndex() const {
  return HEGADBParser::RuleColumnName;
}

void HEGADBParser::ColumnNameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterColumnName(this);
}

void HEGADBParser::ColumnNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitColumnName(this);
}


std::any HEGADBParser::ColumnNameContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitColumnName(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::ColumnNameContext* HEGADBParser::columnName() {
  ColumnNameContext *_localctx = _tracker.createInstance<ColumnNameContext>(_ctx, getState());
  enterRule(_localctx, 24, HEGADBParser::RuleColumnName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(94);
    match(HEGADBParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LiteralContext ------------------------------------------------------------------

HEGADBParser::LiteralContext::LiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::LiteralContext::INTEGER_LITERAL() {
  return getToken(HEGADBParser::INTEGER_LITERAL, 0);
}

tree::TerminalNode* HEGADBParser::LiteralContext::STRING_LITERAL() {
  return getToken(HEGADBParser::STRING_LITERAL, 0);
}

tree::TerminalNode* HEGADBParser::LiteralContext::TRUE() {
  return getToken(HEGADBParser::TRUE, 0);
}

tree::TerminalNode* HEGADBParser::LiteralContext::FALSE() {
  return getToken(HEGADBParser::FALSE, 0);
}


size_t HEGADBParser::LiteralContext::getRuleIndex() const {
  return HEGADBParser::RuleLiteral;
}

void HEGADBParser::LiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLiteral(this);
}

void HEGADBParser::LiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLiteral(this);
}


std::any HEGADBParser::LiteralContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitLiteral(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::LiteralContext* HEGADBParser::literal() {
  LiteralContext *_localctx = _tracker.createInstance<LiteralContext>(_ctx, getState());
  enterRule(_localctx, 26, HEGADBParser::RuleLiteral);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(96);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3146112) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void HEGADBParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  hegadbparserParserInitialize();
#else
  ::antlr4::internal::call_once(hegadbparserParserOnceFlag, hegadbparserParserInitialize);
#endif
}
