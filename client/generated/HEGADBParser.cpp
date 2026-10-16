
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
      "query", "statement", "selectStatement", "createTableStatement", "columnDefinition", 
      "nullability", "dataType", "dropTableStatement", "selectList", "tableName", 
      "whereClause", "booleanExpression", "orExpression", "andExpression", 
      "notExpression", "comparisonExpression", "comparisonOperator", "operand", 
      "columnName", "literal"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "'='", "'!='", "'<='", "'>='", "'<'", "'>'", "'*'", "','", "'('", 
      "')'", "';'"
    },
    std::vector<std::string>{
      "", "SELECT", "FROM", "WHERE", "AND", "OR", "NOT", "TRUE", "FALSE", 
      "NULL_", "CREATE", "DROP", "TABLE", "INTEGER", "FLOAT", "STRING", 
      "BOOLEAN", "EQ", "NEQ", "LTE", "GTE", "LT", "GT", "STAR", "COMMA", 
      "LPAREN", "RPAREN", "SEMICOLON", "INTEGER_LITERAL", "STRING_LITERAL", 
      "IDENTIFIER", "WS", "LINE_COMMENT", "BLOCK_COMMENT"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,33,148,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,1,0,1,0,1,0,1,1,
  	1,1,1,1,3,1,47,8,1,1,2,1,2,1,2,1,2,1,2,3,2,54,8,2,1,2,3,2,57,8,2,1,3,
  	1,3,1,3,1,3,1,3,1,3,1,3,5,3,66,8,3,10,3,12,3,69,9,3,1,3,1,3,3,3,73,8,
  	3,1,4,1,4,1,4,3,4,78,8,4,1,5,1,5,1,5,1,6,1,6,1,7,1,7,1,7,1,7,3,7,89,8,
  	7,1,8,1,8,1,8,1,8,5,8,95,8,8,10,8,12,8,98,9,8,3,8,100,8,8,1,9,1,9,1,10,
  	1,10,1,10,1,11,1,11,1,12,1,12,1,12,5,12,112,8,12,10,12,12,12,115,9,12,
  	1,13,1,13,1,13,5,13,120,8,13,10,13,12,13,123,9,13,1,14,1,14,1,14,1,14,
  	1,14,1,14,1,14,3,14,132,8,14,1,15,1,15,1,15,1,15,1,16,1,16,1,17,1,17,
  	3,17,142,8,17,1,18,1,18,1,19,1,19,1,19,0,0,20,0,2,4,6,8,10,12,14,16,18,
  	20,22,24,26,28,30,32,34,36,38,0,3,1,0,13,16,1,0,17,22,2,0,7,8,28,29,142,
  	0,40,1,0,0,0,2,46,1,0,0,0,4,48,1,0,0,0,6,58,1,0,0,0,8,74,1,0,0,0,10,79,
  	1,0,0,0,12,82,1,0,0,0,14,84,1,0,0,0,16,99,1,0,0,0,18,101,1,0,0,0,20,103,
  	1,0,0,0,22,106,1,0,0,0,24,108,1,0,0,0,26,116,1,0,0,0,28,131,1,0,0,0,30,
  	133,1,0,0,0,32,137,1,0,0,0,34,141,1,0,0,0,36,143,1,0,0,0,38,145,1,0,0,
  	0,40,41,3,2,1,0,41,42,5,0,0,1,42,1,1,0,0,0,43,47,3,4,2,0,44,47,3,6,3,
  	0,45,47,3,14,7,0,46,43,1,0,0,0,46,44,1,0,0,0,46,45,1,0,0,0,47,3,1,0,0,
  	0,48,49,5,1,0,0,49,50,3,16,8,0,50,51,5,2,0,0,51,53,3,18,9,0,52,54,3,20,
  	10,0,53,52,1,0,0,0,53,54,1,0,0,0,54,56,1,0,0,0,55,57,5,27,0,0,56,55,1,
  	0,0,0,56,57,1,0,0,0,57,5,1,0,0,0,58,59,5,10,0,0,59,60,5,12,0,0,60,61,
  	3,18,9,0,61,62,5,25,0,0,62,67,3,8,4,0,63,64,5,24,0,0,64,66,3,8,4,0,65,
  	63,1,0,0,0,66,69,1,0,0,0,67,65,1,0,0,0,67,68,1,0,0,0,68,70,1,0,0,0,69,
  	67,1,0,0,0,70,72,5,26,0,0,71,73,5,27,0,0,72,71,1,0,0,0,72,73,1,0,0,0,
  	73,7,1,0,0,0,74,75,3,36,18,0,75,77,3,12,6,0,76,78,3,10,5,0,77,76,1,0,
  	0,0,77,78,1,0,0,0,78,9,1,0,0,0,79,80,5,6,0,0,80,81,5,9,0,0,81,11,1,0,
  	0,0,82,83,7,0,0,0,83,13,1,0,0,0,84,85,5,11,0,0,85,86,5,12,0,0,86,88,3,
  	18,9,0,87,89,5,27,0,0,88,87,1,0,0,0,88,89,1,0,0,0,89,15,1,0,0,0,90,100,
  	5,23,0,0,91,96,3,36,18,0,92,93,5,24,0,0,93,95,3,36,18,0,94,92,1,0,0,0,
  	95,98,1,0,0,0,96,94,1,0,0,0,96,97,1,0,0,0,97,100,1,0,0,0,98,96,1,0,0,
  	0,99,90,1,0,0,0,99,91,1,0,0,0,100,17,1,0,0,0,101,102,5,30,0,0,102,19,
  	1,0,0,0,103,104,5,3,0,0,104,105,3,22,11,0,105,21,1,0,0,0,106,107,3,24,
  	12,0,107,23,1,0,0,0,108,113,3,26,13,0,109,110,5,5,0,0,110,112,3,26,13,
  	0,111,109,1,0,0,0,112,115,1,0,0,0,113,111,1,0,0,0,113,114,1,0,0,0,114,
  	25,1,0,0,0,115,113,1,0,0,0,116,121,3,28,14,0,117,118,5,4,0,0,118,120,
  	3,28,14,0,119,117,1,0,0,0,120,123,1,0,0,0,121,119,1,0,0,0,121,122,1,0,
  	0,0,122,27,1,0,0,0,123,121,1,0,0,0,124,125,5,6,0,0,125,132,3,28,14,0,
  	126,127,5,25,0,0,127,128,3,22,11,0,128,129,5,26,0,0,129,132,1,0,0,0,130,
  	132,3,30,15,0,131,124,1,0,0,0,131,126,1,0,0,0,131,130,1,0,0,0,132,29,
  	1,0,0,0,133,134,3,34,17,0,134,135,3,32,16,0,135,136,3,34,17,0,136,31,
  	1,0,0,0,137,138,7,1,0,0,138,33,1,0,0,0,139,142,3,36,18,0,140,142,3,38,
  	19,0,141,139,1,0,0,0,141,140,1,0,0,0,142,35,1,0,0,0,143,144,5,30,0,0,
  	144,37,1,0,0,0,145,146,7,2,0,0,146,39,1,0,0,0,13,46,53,56,67,72,77,88,
  	96,99,113,121,131,141
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

HEGADBParser::StatementContext* HEGADBParser::QueryContext::statement() {
  return getRuleContext<HEGADBParser::StatementContext>(0);
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
    setState(40);
    statement();
    setState(41);
    match(HEGADBParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementContext ------------------------------------------------------------------

HEGADBParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HEGADBParser::SelectStatementContext* HEGADBParser::StatementContext::selectStatement() {
  return getRuleContext<HEGADBParser::SelectStatementContext>(0);
}

HEGADBParser::CreateTableStatementContext* HEGADBParser::StatementContext::createTableStatement() {
  return getRuleContext<HEGADBParser::CreateTableStatementContext>(0);
}

HEGADBParser::DropTableStatementContext* HEGADBParser::StatementContext::dropTableStatement() {
  return getRuleContext<HEGADBParser::DropTableStatementContext>(0);
}


size_t HEGADBParser::StatementContext::getRuleIndex() const {
  return HEGADBParser::RuleStatement;
}

void HEGADBParser::StatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void HEGADBParser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatement(this);
}


std::any HEGADBParser::StatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitStatement(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::StatementContext* HEGADBParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 2, HEGADBParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(46);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::SELECT: {
        enterOuterAlt(_localctx, 1);
        setState(43);
        selectStatement();
        break;
      }

      case HEGADBParser::CREATE: {
        enterOuterAlt(_localctx, 2);
        setState(44);
        createTableStatement();
        break;
      }

      case HEGADBParser::DROP: {
        enterOuterAlt(_localctx, 3);
        setState(45);
        dropTableStatement();
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
  enterRule(_localctx, 4, HEGADBParser::RuleSelectStatement);
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
    setState(48);
    match(HEGADBParser::SELECT);
    setState(49);
    selectList();
    setState(50);
    match(HEGADBParser::FROM);
    setState(51);
    tableName();
    setState(53);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::WHERE) {
      setState(52);
      whereClause();
    }
    setState(56);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::SEMICOLON) {
      setState(55);
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

//----------------- CreateTableStatementContext ------------------------------------------------------------------

HEGADBParser::CreateTableStatementContext::CreateTableStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::CreateTableStatementContext::CREATE() {
  return getToken(HEGADBParser::CREATE, 0);
}

tree::TerminalNode* HEGADBParser::CreateTableStatementContext::TABLE() {
  return getToken(HEGADBParser::TABLE, 0);
}

HEGADBParser::TableNameContext* HEGADBParser::CreateTableStatementContext::tableName() {
  return getRuleContext<HEGADBParser::TableNameContext>(0);
}

tree::TerminalNode* HEGADBParser::CreateTableStatementContext::LPAREN() {
  return getToken(HEGADBParser::LPAREN, 0);
}

std::vector<HEGADBParser::ColumnDefinitionContext *> HEGADBParser::CreateTableStatementContext::columnDefinition() {
  return getRuleContexts<HEGADBParser::ColumnDefinitionContext>();
}

HEGADBParser::ColumnDefinitionContext* HEGADBParser::CreateTableStatementContext::columnDefinition(size_t i) {
  return getRuleContext<HEGADBParser::ColumnDefinitionContext>(i);
}

tree::TerminalNode* HEGADBParser::CreateTableStatementContext::RPAREN() {
  return getToken(HEGADBParser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> HEGADBParser::CreateTableStatementContext::COMMA() {
  return getTokens(HEGADBParser::COMMA);
}

tree::TerminalNode* HEGADBParser::CreateTableStatementContext::COMMA(size_t i) {
  return getToken(HEGADBParser::COMMA, i);
}

tree::TerminalNode* HEGADBParser::CreateTableStatementContext::SEMICOLON() {
  return getToken(HEGADBParser::SEMICOLON, 0);
}


size_t HEGADBParser::CreateTableStatementContext::getRuleIndex() const {
  return HEGADBParser::RuleCreateTableStatement;
}

void HEGADBParser::CreateTableStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCreateTableStatement(this);
}

void HEGADBParser::CreateTableStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCreateTableStatement(this);
}


std::any HEGADBParser::CreateTableStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitCreateTableStatement(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::CreateTableStatementContext* HEGADBParser::createTableStatement() {
  CreateTableStatementContext *_localctx = _tracker.createInstance<CreateTableStatementContext>(_ctx, getState());
  enterRule(_localctx, 6, HEGADBParser::RuleCreateTableStatement);
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
    setState(58);
    match(HEGADBParser::CREATE);
    setState(59);
    match(HEGADBParser::TABLE);
    setState(60);
    tableName();
    setState(61);
    match(HEGADBParser::LPAREN);
    setState(62);
    columnDefinition();
    setState(67);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HEGADBParser::COMMA) {
      setState(63);
      match(HEGADBParser::COMMA);
      setState(64);
      columnDefinition();
      setState(69);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(70);
    match(HEGADBParser::RPAREN);
    setState(72);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::SEMICOLON) {
      setState(71);
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

//----------------- ColumnDefinitionContext ------------------------------------------------------------------

HEGADBParser::ColumnDefinitionContext::ColumnDefinitionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HEGADBParser::ColumnNameContext* HEGADBParser::ColumnDefinitionContext::columnName() {
  return getRuleContext<HEGADBParser::ColumnNameContext>(0);
}

HEGADBParser::DataTypeContext* HEGADBParser::ColumnDefinitionContext::dataType() {
  return getRuleContext<HEGADBParser::DataTypeContext>(0);
}

HEGADBParser::NullabilityContext* HEGADBParser::ColumnDefinitionContext::nullability() {
  return getRuleContext<HEGADBParser::NullabilityContext>(0);
}


size_t HEGADBParser::ColumnDefinitionContext::getRuleIndex() const {
  return HEGADBParser::RuleColumnDefinition;
}

void HEGADBParser::ColumnDefinitionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterColumnDefinition(this);
}

void HEGADBParser::ColumnDefinitionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitColumnDefinition(this);
}


std::any HEGADBParser::ColumnDefinitionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitColumnDefinition(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::ColumnDefinitionContext* HEGADBParser::columnDefinition() {
  ColumnDefinitionContext *_localctx = _tracker.createInstance<ColumnDefinitionContext>(_ctx, getState());
  enterRule(_localctx, 8, HEGADBParser::RuleColumnDefinition);
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
    setState(74);
    columnName();
    setState(75);
    dataType();
    setState(77);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::NOT) {
      setState(76);
      nullability();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NullabilityContext ------------------------------------------------------------------

HEGADBParser::NullabilityContext::NullabilityContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::NullabilityContext::NOT() {
  return getToken(HEGADBParser::NOT, 0);
}

tree::TerminalNode* HEGADBParser::NullabilityContext::NULL_() {
  return getToken(HEGADBParser::NULL_, 0);
}


size_t HEGADBParser::NullabilityContext::getRuleIndex() const {
  return HEGADBParser::RuleNullability;
}

void HEGADBParser::NullabilityContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNullability(this);
}

void HEGADBParser::NullabilityContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNullability(this);
}


std::any HEGADBParser::NullabilityContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitNullability(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::NullabilityContext* HEGADBParser::nullability() {
  NullabilityContext *_localctx = _tracker.createInstance<NullabilityContext>(_ctx, getState());
  enterRule(_localctx, 10, HEGADBParser::RuleNullability);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(79);
    match(HEGADBParser::NOT);
    setState(80);
    match(HEGADBParser::NULL_);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DataTypeContext ------------------------------------------------------------------

HEGADBParser::DataTypeContext::DataTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::DataTypeContext::INTEGER() {
  return getToken(HEGADBParser::INTEGER, 0);
}

tree::TerminalNode* HEGADBParser::DataTypeContext::FLOAT() {
  return getToken(HEGADBParser::FLOAT, 0);
}

tree::TerminalNode* HEGADBParser::DataTypeContext::STRING() {
  return getToken(HEGADBParser::STRING, 0);
}

tree::TerminalNode* HEGADBParser::DataTypeContext::BOOLEAN() {
  return getToken(HEGADBParser::BOOLEAN, 0);
}


size_t HEGADBParser::DataTypeContext::getRuleIndex() const {
  return HEGADBParser::RuleDataType;
}

void HEGADBParser::DataTypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDataType(this);
}

void HEGADBParser::DataTypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDataType(this);
}


std::any HEGADBParser::DataTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitDataType(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::DataTypeContext* HEGADBParser::dataType() {
  DataTypeContext *_localctx = _tracker.createInstance<DataTypeContext>(_ctx, getState());
  enterRule(_localctx, 12, HEGADBParser::RuleDataType);
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
    setState(82);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 122880) != 0))) {
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

//----------------- DropTableStatementContext ------------------------------------------------------------------

HEGADBParser::DropTableStatementContext::DropTableStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::DropTableStatementContext::DROP() {
  return getToken(HEGADBParser::DROP, 0);
}

tree::TerminalNode* HEGADBParser::DropTableStatementContext::TABLE() {
  return getToken(HEGADBParser::TABLE, 0);
}

HEGADBParser::TableNameContext* HEGADBParser::DropTableStatementContext::tableName() {
  return getRuleContext<HEGADBParser::TableNameContext>(0);
}

tree::TerminalNode* HEGADBParser::DropTableStatementContext::SEMICOLON() {
  return getToken(HEGADBParser::SEMICOLON, 0);
}


size_t HEGADBParser::DropTableStatementContext::getRuleIndex() const {
  return HEGADBParser::RuleDropTableStatement;
}

void HEGADBParser::DropTableStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDropTableStatement(this);
}

void HEGADBParser::DropTableStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDropTableStatement(this);
}


std::any HEGADBParser::DropTableStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitDropTableStatement(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::DropTableStatementContext* HEGADBParser::dropTableStatement() {
  DropTableStatementContext *_localctx = _tracker.createInstance<DropTableStatementContext>(_ctx, getState());
  enterRule(_localctx, 14, HEGADBParser::RuleDropTableStatement);
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
    setState(84);
    match(HEGADBParser::DROP);
    setState(85);
    match(HEGADBParser::TABLE);
    setState(86);
    tableName();
    setState(88);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::SEMICOLON) {
      setState(87);
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
  enterRule(_localctx, 16, HEGADBParser::RuleSelectList);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(99);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::STAR: {
        enterOuterAlt(_localctx, 1);
        setState(90);
        match(HEGADBParser::STAR);
        break;
      }

      case HEGADBParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 2);
        setState(91);
        columnName();
        setState(96);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == HEGADBParser::COMMA) {
          setState(92);
          match(HEGADBParser::COMMA);
          setState(93);
          columnName();
          setState(98);
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
  enterRule(_localctx, 18, HEGADBParser::RuleTableName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(101);
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
  enterRule(_localctx, 20, HEGADBParser::RuleWhereClause);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(103);
    match(HEGADBParser::WHERE);
    setState(104);
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
  enterRule(_localctx, 22, HEGADBParser::RuleBooleanExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(106);
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
  enterRule(_localctx, 24, HEGADBParser::RuleOrExpression);
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
    setState(108);
    andExpression();
    setState(113);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HEGADBParser::OR) {
      setState(109);
      match(HEGADBParser::OR);
      setState(110);
      andExpression();
      setState(115);
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
  enterRule(_localctx, 26, HEGADBParser::RuleAndExpression);
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
    setState(116);
    notExpression();
    setState(121);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HEGADBParser::AND) {
      setState(117);
      match(HEGADBParser::AND);
      setState(118);
      notExpression();
      setState(123);
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
  enterRule(_localctx, 28, HEGADBParser::RuleNotExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(131);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::NOT: {
        enterOuterAlt(_localctx, 1);
        setState(124);
        match(HEGADBParser::NOT);
        setState(125);
        notExpression();
        break;
      }

      case HEGADBParser::LPAREN: {
        enterOuterAlt(_localctx, 2);
        setState(126);
        match(HEGADBParser::LPAREN);
        setState(127);
        booleanExpression();
        setState(128);
        match(HEGADBParser::RPAREN);
        break;
      }

      case HEGADBParser::TRUE:
      case HEGADBParser::FALSE:
      case HEGADBParser::INTEGER_LITERAL:
      case HEGADBParser::STRING_LITERAL:
      case HEGADBParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 3);
        setState(130);
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
  enterRule(_localctx, 30, HEGADBParser::RuleComparisonExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(133);
    operand();
    setState(134);
    comparisonOperator();
    setState(135);
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
  enterRule(_localctx, 32, HEGADBParser::RuleComparisonOperator);
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
    setState(137);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 8257536) != 0))) {
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
  enterRule(_localctx, 34, HEGADBParser::RuleOperand);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(141);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(139);
        columnName();
        break;
      }

      case HEGADBParser::TRUE:
      case HEGADBParser::FALSE:
      case HEGADBParser::INTEGER_LITERAL:
      case HEGADBParser::STRING_LITERAL: {
        enterOuterAlt(_localctx, 2);
        setState(140);
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
  enterRule(_localctx, 36, HEGADBParser::RuleColumnName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(143);
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
  enterRule(_localctx, 38, HEGADBParser::RuleLiteral);
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
    setState(145);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 805306752) != 0))) {
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
