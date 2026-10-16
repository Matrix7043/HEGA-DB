
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
      "nullability", "dataType", "dropTableStatement", "selectList", "selectItem", 
      "joinClause", "qualifiedColumn", "tableName", "whereClause", "booleanExpression", 
      "orExpression", "andExpression", "notExpression", "comparisonExpression", 
      "comparisonOperator", "operand", "columnName", "literal"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "'='", "'!='", "'<='", "'>='", "'<'", "'>'", "'*'", "','", 
      "'.'", "'('", "')'", "';'"
    },
    std::vector<std::string>{
      "", "SELECT", "FROM", "WHERE", "AND", "OR", "NOT", "TRUE", "FALSE", 
      "NULL_", "CREATE", "DROP", "TABLE", "INNER", "JOIN", "ON", "INTEGER", 
      "FLOAT", "STRING", "BOOLEAN", "EQ", "NEQ", "LTE", "GTE", "LT", "GT", 
      "STAR", "COMMA", "DOT", "LPAREN", "RPAREN", "SEMICOLON", "INTEGER_LITERAL", 
      "STRING_LITERAL", "IDENTIFIER", "WS", "LINE_COMMENT", "BLOCK_COMMENT"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,37,173,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,1,0,1,0,1,0,1,1,1,1,1,1,3,1,53,8,1,1,2,1,2,1,2,1,2,1,2,1,
  	2,3,2,61,8,2,1,2,3,2,64,8,2,1,3,1,3,1,3,1,3,1,3,1,3,1,3,5,3,73,8,3,10,
  	3,12,3,76,9,3,1,3,1,3,3,3,80,8,3,1,4,1,4,1,4,3,4,85,8,4,1,5,1,5,1,5,1,
  	6,1,6,1,7,1,7,1,7,1,7,3,7,96,8,7,1,8,1,8,1,8,1,8,5,8,102,8,8,10,8,12,
  	8,105,9,8,3,8,107,8,8,1,9,1,9,3,9,111,8,9,1,10,3,10,114,8,10,1,10,1,10,
  	1,10,1,10,1,10,1,10,1,10,1,11,1,11,1,11,1,11,1,12,1,12,1,13,1,13,1,13,
  	1,14,1,14,1,15,1,15,1,15,5,15,137,8,15,10,15,12,15,140,9,15,1,16,1,16,
  	1,16,5,16,145,8,16,10,16,12,16,148,9,16,1,17,1,17,1,17,1,17,1,17,1,17,
  	1,17,3,17,157,8,17,1,18,1,18,1,18,1,18,1,19,1,19,1,20,1,20,3,20,167,8,
  	20,1,21,1,21,1,22,1,22,1,22,0,0,23,0,2,4,6,8,10,12,14,16,18,20,22,24,
  	26,28,30,32,34,36,38,40,42,44,0,3,1,0,16,19,1,0,20,25,2,0,7,8,32,33,167,
  	0,46,1,0,0,0,2,52,1,0,0,0,4,54,1,0,0,0,6,65,1,0,0,0,8,81,1,0,0,0,10,86,
  	1,0,0,0,12,89,1,0,0,0,14,91,1,0,0,0,16,106,1,0,0,0,18,110,1,0,0,0,20,
  	113,1,0,0,0,22,122,1,0,0,0,24,126,1,0,0,0,26,128,1,0,0,0,28,131,1,0,0,
  	0,30,133,1,0,0,0,32,141,1,0,0,0,34,156,1,0,0,0,36,158,1,0,0,0,38,162,
  	1,0,0,0,40,166,1,0,0,0,42,168,1,0,0,0,44,170,1,0,0,0,46,47,3,2,1,0,47,
  	48,5,0,0,1,48,1,1,0,0,0,49,53,3,4,2,0,50,53,3,6,3,0,51,53,3,14,7,0,52,
  	49,1,0,0,0,52,50,1,0,0,0,52,51,1,0,0,0,53,3,1,0,0,0,54,55,5,1,0,0,55,
  	56,3,16,8,0,56,57,5,2,0,0,57,60,3,24,12,0,58,61,3,20,10,0,59,61,3,26,
  	13,0,60,58,1,0,0,0,60,59,1,0,0,0,60,61,1,0,0,0,61,63,1,0,0,0,62,64,5,
  	31,0,0,63,62,1,0,0,0,63,64,1,0,0,0,64,5,1,0,0,0,65,66,5,10,0,0,66,67,
  	5,12,0,0,67,68,3,24,12,0,68,69,5,29,0,0,69,74,3,8,4,0,70,71,5,27,0,0,
  	71,73,3,8,4,0,72,70,1,0,0,0,73,76,1,0,0,0,74,72,1,0,0,0,74,75,1,0,0,0,
  	75,77,1,0,0,0,76,74,1,0,0,0,77,79,5,30,0,0,78,80,5,31,0,0,79,78,1,0,0,
  	0,79,80,1,0,0,0,80,7,1,0,0,0,81,82,3,42,21,0,82,84,3,12,6,0,83,85,3,10,
  	5,0,84,83,1,0,0,0,84,85,1,0,0,0,85,9,1,0,0,0,86,87,5,6,0,0,87,88,5,9,
  	0,0,88,11,1,0,0,0,89,90,7,0,0,0,90,13,1,0,0,0,91,92,5,11,0,0,92,93,5,
  	12,0,0,93,95,3,24,12,0,94,96,5,31,0,0,95,94,1,0,0,0,95,96,1,0,0,0,96,
  	15,1,0,0,0,97,107,5,26,0,0,98,103,3,18,9,0,99,100,5,27,0,0,100,102,3,
  	18,9,0,101,99,1,0,0,0,102,105,1,0,0,0,103,101,1,0,0,0,103,104,1,0,0,0,
  	104,107,1,0,0,0,105,103,1,0,0,0,106,97,1,0,0,0,106,98,1,0,0,0,107,17,
  	1,0,0,0,108,111,3,22,11,0,109,111,3,42,21,0,110,108,1,0,0,0,110,109,1,
  	0,0,0,111,19,1,0,0,0,112,114,5,13,0,0,113,112,1,0,0,0,113,114,1,0,0,0,
  	114,115,1,0,0,0,115,116,5,14,0,0,116,117,3,24,12,0,117,118,5,15,0,0,118,
  	119,3,22,11,0,119,120,5,20,0,0,120,121,3,22,11,0,121,21,1,0,0,0,122,123,
  	3,24,12,0,123,124,5,28,0,0,124,125,3,42,21,0,125,23,1,0,0,0,126,127,5,
  	34,0,0,127,25,1,0,0,0,128,129,5,3,0,0,129,130,3,28,14,0,130,27,1,0,0,
  	0,131,132,3,30,15,0,132,29,1,0,0,0,133,138,3,32,16,0,134,135,5,5,0,0,
  	135,137,3,32,16,0,136,134,1,0,0,0,137,140,1,0,0,0,138,136,1,0,0,0,138,
  	139,1,0,0,0,139,31,1,0,0,0,140,138,1,0,0,0,141,146,3,34,17,0,142,143,
  	5,4,0,0,143,145,3,34,17,0,144,142,1,0,0,0,145,148,1,0,0,0,146,144,1,0,
  	0,0,146,147,1,0,0,0,147,33,1,0,0,0,148,146,1,0,0,0,149,150,5,6,0,0,150,
  	157,3,34,17,0,151,152,5,29,0,0,152,153,3,28,14,0,153,154,5,30,0,0,154,
  	157,1,0,0,0,155,157,3,36,18,0,156,149,1,0,0,0,156,151,1,0,0,0,156,155,
  	1,0,0,0,157,35,1,0,0,0,158,159,3,40,20,0,159,160,3,38,19,0,160,161,3,
  	40,20,0,161,37,1,0,0,0,162,163,7,1,0,0,163,39,1,0,0,0,164,167,3,42,21,
  	0,165,167,3,44,22,0,166,164,1,0,0,0,166,165,1,0,0,0,167,41,1,0,0,0,168,
  	169,5,34,0,0,169,43,1,0,0,0,170,171,7,2,0,0,171,45,1,0,0,0,15,52,60,63,
  	74,79,84,95,103,106,110,113,138,146,156,166
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
    setState(46);
    statement();
    setState(47);
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
    setState(52);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::SELECT: {
        enterOuterAlt(_localctx, 1);
        setState(49);
        selectStatement();
        break;
      }

      case HEGADBParser::CREATE: {
        enterOuterAlt(_localctx, 2);
        setState(50);
        createTableStatement();
        break;
      }

      case HEGADBParser::DROP: {
        enterOuterAlt(_localctx, 3);
        setState(51);
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

HEGADBParser::JoinClauseContext* HEGADBParser::SelectStatementContext::joinClause() {
  return getRuleContext<HEGADBParser::JoinClauseContext>(0);
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
    setState(54);
    match(HEGADBParser::SELECT);
    setState(55);
    selectList();
    setState(56);
    match(HEGADBParser::FROM);
    setState(57);
    tableName();
    setState(60);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::INNER:
      case HEGADBParser::JOIN: {
        setState(58);
        joinClause();
        break;
      }

      case HEGADBParser::WHERE: {
        setState(59);
        whereClause();
        break;
      }

      case HEGADBParser::EOF:
      case HEGADBParser::SEMICOLON: {
        break;
      }

    default:
      break;
    }
    setState(63);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::SEMICOLON) {
      setState(62);
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
    setState(65);
    match(HEGADBParser::CREATE);
    setState(66);
    match(HEGADBParser::TABLE);
    setState(67);
    tableName();
    setState(68);
    match(HEGADBParser::LPAREN);
    setState(69);
    columnDefinition();
    setState(74);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HEGADBParser::COMMA) {
      setState(70);
      match(HEGADBParser::COMMA);
      setState(71);
      columnDefinition();
      setState(76);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(77);
    match(HEGADBParser::RPAREN);
    setState(79);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::SEMICOLON) {
      setState(78);
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
    setState(81);
    columnName();
    setState(82);
    dataType();
    setState(84);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::NOT) {
      setState(83);
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
    setState(86);
    match(HEGADBParser::NOT);
    setState(87);
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
    setState(89);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 983040) != 0))) {
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
    setState(91);
    match(HEGADBParser::DROP);
    setState(92);
    match(HEGADBParser::TABLE);
    setState(93);
    tableName();
    setState(95);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::SEMICOLON) {
      setState(94);
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

std::vector<HEGADBParser::SelectItemContext *> HEGADBParser::SelectListContext::selectItem() {
  return getRuleContexts<HEGADBParser::SelectItemContext>();
}

HEGADBParser::SelectItemContext* HEGADBParser::SelectListContext::selectItem(size_t i) {
  return getRuleContext<HEGADBParser::SelectItemContext>(i);
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
    setState(106);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::STAR: {
        enterOuterAlt(_localctx, 1);
        setState(97);
        match(HEGADBParser::STAR);
        break;
      }

      case HEGADBParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 2);
        setState(98);
        selectItem();
        setState(103);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == HEGADBParser::COMMA) {
          setState(99);
          match(HEGADBParser::COMMA);
          setState(100);
          selectItem();
          setState(105);
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

//----------------- SelectItemContext ------------------------------------------------------------------

HEGADBParser::SelectItemContext::SelectItemContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HEGADBParser::QualifiedColumnContext* HEGADBParser::SelectItemContext::qualifiedColumn() {
  return getRuleContext<HEGADBParser::QualifiedColumnContext>(0);
}

HEGADBParser::ColumnNameContext* HEGADBParser::SelectItemContext::columnName() {
  return getRuleContext<HEGADBParser::ColumnNameContext>(0);
}


size_t HEGADBParser::SelectItemContext::getRuleIndex() const {
  return HEGADBParser::RuleSelectItem;
}

void HEGADBParser::SelectItemContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSelectItem(this);
}

void HEGADBParser::SelectItemContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSelectItem(this);
}


std::any HEGADBParser::SelectItemContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitSelectItem(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::SelectItemContext* HEGADBParser::selectItem() {
  SelectItemContext *_localctx = _tracker.createInstance<SelectItemContext>(_ctx, getState());
  enterRule(_localctx, 18, HEGADBParser::RuleSelectItem);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(110);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(108);
      qualifiedColumn();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(109);
      columnName();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- JoinClauseContext ------------------------------------------------------------------

HEGADBParser::JoinClauseContext::JoinClauseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::JoinClauseContext::JOIN() {
  return getToken(HEGADBParser::JOIN, 0);
}

HEGADBParser::TableNameContext* HEGADBParser::JoinClauseContext::tableName() {
  return getRuleContext<HEGADBParser::TableNameContext>(0);
}

tree::TerminalNode* HEGADBParser::JoinClauseContext::ON() {
  return getToken(HEGADBParser::ON, 0);
}

std::vector<HEGADBParser::QualifiedColumnContext *> HEGADBParser::JoinClauseContext::qualifiedColumn() {
  return getRuleContexts<HEGADBParser::QualifiedColumnContext>();
}

HEGADBParser::QualifiedColumnContext* HEGADBParser::JoinClauseContext::qualifiedColumn(size_t i) {
  return getRuleContext<HEGADBParser::QualifiedColumnContext>(i);
}

tree::TerminalNode* HEGADBParser::JoinClauseContext::EQ() {
  return getToken(HEGADBParser::EQ, 0);
}

tree::TerminalNode* HEGADBParser::JoinClauseContext::INNER() {
  return getToken(HEGADBParser::INNER, 0);
}


size_t HEGADBParser::JoinClauseContext::getRuleIndex() const {
  return HEGADBParser::RuleJoinClause;
}

void HEGADBParser::JoinClauseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterJoinClause(this);
}

void HEGADBParser::JoinClauseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitJoinClause(this);
}


std::any HEGADBParser::JoinClauseContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitJoinClause(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::JoinClauseContext* HEGADBParser::joinClause() {
  JoinClauseContext *_localctx = _tracker.createInstance<JoinClauseContext>(_ctx, getState());
  enterRule(_localctx, 20, HEGADBParser::RuleJoinClause);
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
    setState(113);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::INNER) {
      setState(112);
      match(HEGADBParser::INNER);
    }
    setState(115);
    match(HEGADBParser::JOIN);
    setState(116);
    tableName();
    setState(117);
    match(HEGADBParser::ON);
    setState(118);
    qualifiedColumn();
    setState(119);
    match(HEGADBParser::EQ);
    setState(120);
    qualifiedColumn();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- QualifiedColumnContext ------------------------------------------------------------------

HEGADBParser::QualifiedColumnContext::QualifiedColumnContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HEGADBParser::TableNameContext* HEGADBParser::QualifiedColumnContext::tableName() {
  return getRuleContext<HEGADBParser::TableNameContext>(0);
}

tree::TerminalNode* HEGADBParser::QualifiedColumnContext::DOT() {
  return getToken(HEGADBParser::DOT, 0);
}

HEGADBParser::ColumnNameContext* HEGADBParser::QualifiedColumnContext::columnName() {
  return getRuleContext<HEGADBParser::ColumnNameContext>(0);
}


size_t HEGADBParser::QualifiedColumnContext::getRuleIndex() const {
  return HEGADBParser::RuleQualifiedColumn;
}

void HEGADBParser::QualifiedColumnContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterQualifiedColumn(this);
}

void HEGADBParser::QualifiedColumnContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitQualifiedColumn(this);
}


std::any HEGADBParser::QualifiedColumnContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitQualifiedColumn(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::QualifiedColumnContext* HEGADBParser::qualifiedColumn() {
  QualifiedColumnContext *_localctx = _tracker.createInstance<QualifiedColumnContext>(_ctx, getState());
  enterRule(_localctx, 22, HEGADBParser::RuleQualifiedColumn);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(122);
    tableName();
    setState(123);
    match(HEGADBParser::DOT);
    setState(124);
    columnName();
   
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
  enterRule(_localctx, 24, HEGADBParser::RuleTableName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(126);
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
  enterRule(_localctx, 26, HEGADBParser::RuleWhereClause);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(128);
    match(HEGADBParser::WHERE);
    setState(129);
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
  enterRule(_localctx, 28, HEGADBParser::RuleBooleanExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(131);
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
  enterRule(_localctx, 30, HEGADBParser::RuleOrExpression);
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
    setState(133);
    andExpression();
    setState(138);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HEGADBParser::OR) {
      setState(134);
      match(HEGADBParser::OR);
      setState(135);
      andExpression();
      setState(140);
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
  enterRule(_localctx, 32, HEGADBParser::RuleAndExpression);
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
    setState(141);
    notExpression();
    setState(146);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HEGADBParser::AND) {
      setState(142);
      match(HEGADBParser::AND);
      setState(143);
      notExpression();
      setState(148);
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
  enterRule(_localctx, 34, HEGADBParser::RuleNotExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(156);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::NOT: {
        enterOuterAlt(_localctx, 1);
        setState(149);
        match(HEGADBParser::NOT);
        setState(150);
        notExpression();
        break;
      }

      case HEGADBParser::LPAREN: {
        enterOuterAlt(_localctx, 2);
        setState(151);
        match(HEGADBParser::LPAREN);
        setState(152);
        booleanExpression();
        setState(153);
        match(HEGADBParser::RPAREN);
        break;
      }

      case HEGADBParser::TRUE:
      case HEGADBParser::FALSE:
      case HEGADBParser::INTEGER_LITERAL:
      case HEGADBParser::STRING_LITERAL:
      case HEGADBParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 3);
        setState(155);
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
  enterRule(_localctx, 36, HEGADBParser::RuleComparisonExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(158);
    operand();
    setState(159);
    comparisonOperator();
    setState(160);
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
  enterRule(_localctx, 38, HEGADBParser::RuleComparisonOperator);
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
    setState(162);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 66060288) != 0))) {
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
  enterRule(_localctx, 40, HEGADBParser::RuleOperand);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(166);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(164);
        columnName();
        break;
      }

      case HEGADBParser::TRUE:
      case HEGADBParser::FALSE:
      case HEGADBParser::INTEGER_LITERAL:
      case HEGADBParser::STRING_LITERAL: {
        enterOuterAlt(_localctx, 2);
        setState(165);
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
  enterRule(_localctx, 42, HEGADBParser::RuleColumnName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(168);
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
  enterRule(_localctx, 44, HEGADBParser::RuleLiteral);
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
    setState(170);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 12884902272) != 0))) {
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
