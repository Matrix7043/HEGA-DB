
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
      "nullability", "dataType", "dropTableStatement", "insertStatement", 
      "valueRow", "insertValue", "selectList", "selectItem", "joinClause", 
      "qualifiedColumn", "tableName", "whereClause", "booleanExpression", 
      "orExpression", "andExpression", "notExpression", "comparisonExpression", 
      "comparisonOperator", "operand", "columnName", "literal"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "'='", "'!='", "'<='", "'>='", "'<'", "'>'", 
      "'*'", "','", "'.'", "'('", "')'", "';'"
    },
    std::vector<std::string>{
      "", "SELECT", "FROM", "WHERE", "AND", "OR", "NOT", "TRUE", "FALSE", 
      "NULL_", "CREATE", "DROP", "TABLE", "INSERT", "INTO", "VALUES", "INNER", 
      "JOIN", "ON", "INTEGER", "FLOAT", "STRING", "BOOLEAN", "EQ", "NEQ", 
      "LTE", "GTE", "LT", "GT", "STAR", "COMMA", "DOT", "LPAREN", "RPAREN", 
      "SEMICOLON", "INTEGER_LITERAL", "STRING_LITERAL", "IDENTIFIER", "WS", 
      "LINE_COMMENT", "BLOCK_COMMENT"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,40,223,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,1,0,1,0,1,0,1,1,1,1,1,1,1,
  	1,3,1,60,8,1,1,2,1,2,1,2,1,2,1,2,1,2,3,2,68,8,2,1,2,3,2,71,8,2,1,3,1,
  	3,1,3,1,3,1,3,1,3,1,3,5,3,80,8,3,10,3,12,3,83,9,3,1,3,1,3,3,3,87,8,3,
  	1,4,1,4,1,4,3,4,92,8,4,1,5,1,5,1,5,1,6,1,6,1,7,1,7,1,7,1,7,3,7,103,8,
  	7,1,8,1,8,1,8,1,8,1,8,1,8,1,8,5,8,112,8,8,10,8,12,8,115,9,8,1,8,1,8,3,
  	8,119,8,8,1,8,1,8,1,8,1,8,5,8,125,8,8,10,8,12,8,128,9,8,1,8,3,8,131,8,
  	8,1,9,1,9,1,9,1,9,5,9,137,8,9,10,9,12,9,140,9,9,1,9,1,9,1,10,1,10,3,10,
  	146,8,10,1,11,1,11,1,11,1,11,5,11,152,8,11,10,11,12,11,155,9,11,3,11,
  	157,8,11,1,12,1,12,3,12,161,8,12,1,13,3,13,164,8,13,1,13,1,13,1,13,1,
  	13,1,13,1,13,1,13,1,14,1,14,1,14,1,14,1,15,1,15,1,16,1,16,1,16,1,17,1,
  	17,1,18,1,18,1,18,5,18,187,8,18,10,18,12,18,190,9,18,1,19,1,19,1,19,5,
  	19,195,8,19,10,19,12,19,198,9,19,1,20,1,20,1,20,1,20,1,20,1,20,1,20,3,
  	20,207,8,20,1,21,1,21,1,21,1,21,1,22,1,22,1,23,1,23,3,23,217,8,23,1,24,
  	1,24,1,25,1,25,1,25,0,0,26,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,
  	32,34,36,38,40,42,44,46,48,50,0,3,1,0,19,22,1,0,23,28,2,0,7,8,35,36,221,
  	0,52,1,0,0,0,2,59,1,0,0,0,4,61,1,0,0,0,6,72,1,0,0,0,8,88,1,0,0,0,10,93,
  	1,0,0,0,12,96,1,0,0,0,14,98,1,0,0,0,16,104,1,0,0,0,18,132,1,0,0,0,20,
  	145,1,0,0,0,22,156,1,0,0,0,24,160,1,0,0,0,26,163,1,0,0,0,28,172,1,0,0,
  	0,30,176,1,0,0,0,32,178,1,0,0,0,34,181,1,0,0,0,36,183,1,0,0,0,38,191,
  	1,0,0,0,40,206,1,0,0,0,42,208,1,0,0,0,44,212,1,0,0,0,46,216,1,0,0,0,48,
  	218,1,0,0,0,50,220,1,0,0,0,52,53,3,2,1,0,53,54,5,0,0,1,54,1,1,0,0,0,55,
  	60,3,4,2,0,56,60,3,6,3,0,57,60,3,14,7,0,58,60,3,16,8,0,59,55,1,0,0,0,
  	59,56,1,0,0,0,59,57,1,0,0,0,59,58,1,0,0,0,60,3,1,0,0,0,61,62,5,1,0,0,
  	62,63,3,22,11,0,63,64,5,2,0,0,64,67,3,30,15,0,65,68,3,26,13,0,66,68,3,
  	32,16,0,67,65,1,0,0,0,67,66,1,0,0,0,67,68,1,0,0,0,68,70,1,0,0,0,69,71,
  	5,34,0,0,70,69,1,0,0,0,70,71,1,0,0,0,71,5,1,0,0,0,72,73,5,10,0,0,73,74,
  	5,12,0,0,74,75,3,30,15,0,75,76,5,32,0,0,76,81,3,8,4,0,77,78,5,30,0,0,
  	78,80,3,8,4,0,79,77,1,0,0,0,80,83,1,0,0,0,81,79,1,0,0,0,81,82,1,0,0,0,
  	82,84,1,0,0,0,83,81,1,0,0,0,84,86,5,33,0,0,85,87,5,34,0,0,86,85,1,0,0,
  	0,86,87,1,0,0,0,87,7,1,0,0,0,88,89,3,48,24,0,89,91,3,12,6,0,90,92,3,10,
  	5,0,91,90,1,0,0,0,91,92,1,0,0,0,92,9,1,0,0,0,93,94,5,6,0,0,94,95,5,9,
  	0,0,95,11,1,0,0,0,96,97,7,0,0,0,97,13,1,0,0,0,98,99,5,11,0,0,99,100,5,
  	12,0,0,100,102,3,30,15,0,101,103,5,34,0,0,102,101,1,0,0,0,102,103,1,0,
  	0,0,103,15,1,0,0,0,104,105,5,13,0,0,105,106,5,14,0,0,106,118,3,30,15,
  	0,107,108,5,32,0,0,108,113,3,48,24,0,109,110,5,30,0,0,110,112,3,48,24,
  	0,111,109,1,0,0,0,112,115,1,0,0,0,113,111,1,0,0,0,113,114,1,0,0,0,114,
  	116,1,0,0,0,115,113,1,0,0,0,116,117,5,33,0,0,117,119,1,0,0,0,118,107,
  	1,0,0,0,118,119,1,0,0,0,119,120,1,0,0,0,120,121,5,15,0,0,121,126,3,18,
  	9,0,122,123,5,30,0,0,123,125,3,18,9,0,124,122,1,0,0,0,125,128,1,0,0,0,
  	126,124,1,0,0,0,126,127,1,0,0,0,127,130,1,0,0,0,128,126,1,0,0,0,129,131,
  	5,34,0,0,130,129,1,0,0,0,130,131,1,0,0,0,131,17,1,0,0,0,132,133,5,32,
  	0,0,133,138,3,20,10,0,134,135,5,30,0,0,135,137,3,20,10,0,136,134,1,0,
  	0,0,137,140,1,0,0,0,138,136,1,0,0,0,138,139,1,0,0,0,139,141,1,0,0,0,140,
  	138,1,0,0,0,141,142,5,33,0,0,142,19,1,0,0,0,143,146,3,50,25,0,144,146,
  	5,9,0,0,145,143,1,0,0,0,145,144,1,0,0,0,146,21,1,0,0,0,147,157,5,29,0,
  	0,148,153,3,24,12,0,149,150,5,30,0,0,150,152,3,24,12,0,151,149,1,0,0,
  	0,152,155,1,0,0,0,153,151,1,0,0,0,153,154,1,0,0,0,154,157,1,0,0,0,155,
  	153,1,0,0,0,156,147,1,0,0,0,156,148,1,0,0,0,157,23,1,0,0,0,158,161,3,
  	28,14,0,159,161,3,48,24,0,160,158,1,0,0,0,160,159,1,0,0,0,161,25,1,0,
  	0,0,162,164,5,16,0,0,163,162,1,0,0,0,163,164,1,0,0,0,164,165,1,0,0,0,
  	165,166,5,17,0,0,166,167,3,30,15,0,167,168,5,18,0,0,168,169,3,28,14,0,
  	169,170,5,23,0,0,170,171,3,28,14,0,171,27,1,0,0,0,172,173,3,30,15,0,173,
  	174,5,31,0,0,174,175,3,48,24,0,175,29,1,0,0,0,176,177,5,37,0,0,177,31,
  	1,0,0,0,178,179,5,3,0,0,179,180,3,34,17,0,180,33,1,0,0,0,181,182,3,36,
  	18,0,182,35,1,0,0,0,183,188,3,38,19,0,184,185,5,5,0,0,185,187,3,38,19,
  	0,186,184,1,0,0,0,187,190,1,0,0,0,188,186,1,0,0,0,188,189,1,0,0,0,189,
  	37,1,0,0,0,190,188,1,0,0,0,191,196,3,40,20,0,192,193,5,4,0,0,193,195,
  	3,40,20,0,194,192,1,0,0,0,195,198,1,0,0,0,196,194,1,0,0,0,196,197,1,0,
  	0,0,197,39,1,0,0,0,198,196,1,0,0,0,199,200,5,6,0,0,200,207,3,40,20,0,
  	201,202,5,32,0,0,202,203,3,34,17,0,203,204,5,33,0,0,204,207,1,0,0,0,205,
  	207,3,42,21,0,206,199,1,0,0,0,206,201,1,0,0,0,206,205,1,0,0,0,207,41,
  	1,0,0,0,208,209,3,46,23,0,209,210,3,44,22,0,210,211,3,46,23,0,211,43,
  	1,0,0,0,212,213,7,1,0,0,213,45,1,0,0,0,214,217,3,48,24,0,215,217,3,50,
  	25,0,216,214,1,0,0,0,216,215,1,0,0,0,217,47,1,0,0,0,218,219,5,37,0,0,
  	219,49,1,0,0,0,220,221,7,2,0,0,221,51,1,0,0,0,21,59,67,70,81,86,91,102,
  	113,118,126,130,138,145,153,156,160,163,188,196,206,216
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
    setState(52);
    statement();
    setState(53);
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

HEGADBParser::InsertStatementContext* HEGADBParser::StatementContext::insertStatement() {
  return getRuleContext<HEGADBParser::InsertStatementContext>(0);
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
    setState(59);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::SELECT: {
        enterOuterAlt(_localctx, 1);
        setState(55);
        selectStatement();
        break;
      }

      case HEGADBParser::CREATE: {
        enterOuterAlt(_localctx, 2);
        setState(56);
        createTableStatement();
        break;
      }

      case HEGADBParser::DROP: {
        enterOuterAlt(_localctx, 3);
        setState(57);
        dropTableStatement();
        break;
      }

      case HEGADBParser::INSERT: {
        enterOuterAlt(_localctx, 4);
        setState(58);
        insertStatement();
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
    setState(61);
    match(HEGADBParser::SELECT);
    setState(62);
    selectList();
    setState(63);
    match(HEGADBParser::FROM);
    setState(64);
    tableName();
    setState(67);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::INNER:
      case HEGADBParser::JOIN: {
        setState(65);
        joinClause();
        break;
      }

      case HEGADBParser::WHERE: {
        setState(66);
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
    setState(70);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::SEMICOLON) {
      setState(69);
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
    setState(72);
    match(HEGADBParser::CREATE);
    setState(73);
    match(HEGADBParser::TABLE);
    setState(74);
    tableName();
    setState(75);
    match(HEGADBParser::LPAREN);
    setState(76);
    columnDefinition();
    setState(81);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HEGADBParser::COMMA) {
      setState(77);
      match(HEGADBParser::COMMA);
      setState(78);
      columnDefinition();
      setState(83);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(84);
    match(HEGADBParser::RPAREN);
    setState(86);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::SEMICOLON) {
      setState(85);
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
    setState(88);
    columnName();
    setState(89);
    dataType();
    setState(91);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::NOT) {
      setState(90);
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
    setState(93);
    match(HEGADBParser::NOT);
    setState(94);
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
    setState(96);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 7864320) != 0))) {
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
    setState(98);
    match(HEGADBParser::DROP);
    setState(99);
    match(HEGADBParser::TABLE);
    setState(100);
    tableName();
    setState(102);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::SEMICOLON) {
      setState(101);
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

//----------------- InsertStatementContext ------------------------------------------------------------------

HEGADBParser::InsertStatementContext::InsertStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::InsertStatementContext::INSERT() {
  return getToken(HEGADBParser::INSERT, 0);
}

tree::TerminalNode* HEGADBParser::InsertStatementContext::INTO() {
  return getToken(HEGADBParser::INTO, 0);
}

HEGADBParser::TableNameContext* HEGADBParser::InsertStatementContext::tableName() {
  return getRuleContext<HEGADBParser::TableNameContext>(0);
}

tree::TerminalNode* HEGADBParser::InsertStatementContext::VALUES() {
  return getToken(HEGADBParser::VALUES, 0);
}

std::vector<HEGADBParser::ValueRowContext *> HEGADBParser::InsertStatementContext::valueRow() {
  return getRuleContexts<HEGADBParser::ValueRowContext>();
}

HEGADBParser::ValueRowContext* HEGADBParser::InsertStatementContext::valueRow(size_t i) {
  return getRuleContext<HEGADBParser::ValueRowContext>(i);
}

tree::TerminalNode* HEGADBParser::InsertStatementContext::LPAREN() {
  return getToken(HEGADBParser::LPAREN, 0);
}

std::vector<HEGADBParser::ColumnNameContext *> HEGADBParser::InsertStatementContext::columnName() {
  return getRuleContexts<HEGADBParser::ColumnNameContext>();
}

HEGADBParser::ColumnNameContext* HEGADBParser::InsertStatementContext::columnName(size_t i) {
  return getRuleContext<HEGADBParser::ColumnNameContext>(i);
}

tree::TerminalNode* HEGADBParser::InsertStatementContext::RPAREN() {
  return getToken(HEGADBParser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> HEGADBParser::InsertStatementContext::COMMA() {
  return getTokens(HEGADBParser::COMMA);
}

tree::TerminalNode* HEGADBParser::InsertStatementContext::COMMA(size_t i) {
  return getToken(HEGADBParser::COMMA, i);
}

tree::TerminalNode* HEGADBParser::InsertStatementContext::SEMICOLON() {
  return getToken(HEGADBParser::SEMICOLON, 0);
}


size_t HEGADBParser::InsertStatementContext::getRuleIndex() const {
  return HEGADBParser::RuleInsertStatement;
}

void HEGADBParser::InsertStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInsertStatement(this);
}

void HEGADBParser::InsertStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInsertStatement(this);
}


std::any HEGADBParser::InsertStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitInsertStatement(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::InsertStatementContext* HEGADBParser::insertStatement() {
  InsertStatementContext *_localctx = _tracker.createInstance<InsertStatementContext>(_ctx, getState());
  enterRule(_localctx, 16, HEGADBParser::RuleInsertStatement);
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
    setState(104);
    match(HEGADBParser::INSERT);
    setState(105);
    match(HEGADBParser::INTO);
    setState(106);
    tableName();
    setState(118);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::LPAREN) {
      setState(107);
      match(HEGADBParser::LPAREN);
      setState(108);
      columnName();
      setState(113);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == HEGADBParser::COMMA) {
        setState(109);
        match(HEGADBParser::COMMA);
        setState(110);
        columnName();
        setState(115);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(116);
      match(HEGADBParser::RPAREN);
    }
    setState(120);
    match(HEGADBParser::VALUES);
    setState(121);
    valueRow();
    setState(126);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HEGADBParser::COMMA) {
      setState(122);
      match(HEGADBParser::COMMA);
      setState(123);
      valueRow();
      setState(128);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(130);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::SEMICOLON) {
      setState(129);
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

//----------------- ValueRowContext ------------------------------------------------------------------

HEGADBParser::ValueRowContext::ValueRowContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* HEGADBParser::ValueRowContext::LPAREN() {
  return getToken(HEGADBParser::LPAREN, 0);
}

std::vector<HEGADBParser::InsertValueContext *> HEGADBParser::ValueRowContext::insertValue() {
  return getRuleContexts<HEGADBParser::InsertValueContext>();
}

HEGADBParser::InsertValueContext* HEGADBParser::ValueRowContext::insertValue(size_t i) {
  return getRuleContext<HEGADBParser::InsertValueContext>(i);
}

tree::TerminalNode* HEGADBParser::ValueRowContext::RPAREN() {
  return getToken(HEGADBParser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> HEGADBParser::ValueRowContext::COMMA() {
  return getTokens(HEGADBParser::COMMA);
}

tree::TerminalNode* HEGADBParser::ValueRowContext::COMMA(size_t i) {
  return getToken(HEGADBParser::COMMA, i);
}


size_t HEGADBParser::ValueRowContext::getRuleIndex() const {
  return HEGADBParser::RuleValueRow;
}

void HEGADBParser::ValueRowContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValueRow(this);
}

void HEGADBParser::ValueRowContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValueRow(this);
}


std::any HEGADBParser::ValueRowContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitValueRow(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::ValueRowContext* HEGADBParser::valueRow() {
  ValueRowContext *_localctx = _tracker.createInstance<ValueRowContext>(_ctx, getState());
  enterRule(_localctx, 18, HEGADBParser::RuleValueRow);
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
    setState(132);
    match(HEGADBParser::LPAREN);
    setState(133);
    insertValue();
    setState(138);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HEGADBParser::COMMA) {
      setState(134);
      match(HEGADBParser::COMMA);
      setState(135);
      insertValue();
      setState(140);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(141);
    match(HEGADBParser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InsertValueContext ------------------------------------------------------------------

HEGADBParser::InsertValueContext::InsertValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

HEGADBParser::LiteralContext* HEGADBParser::InsertValueContext::literal() {
  return getRuleContext<HEGADBParser::LiteralContext>(0);
}

tree::TerminalNode* HEGADBParser::InsertValueContext::NULL_() {
  return getToken(HEGADBParser::NULL_, 0);
}


size_t HEGADBParser::InsertValueContext::getRuleIndex() const {
  return HEGADBParser::RuleInsertValue;
}

void HEGADBParser::InsertValueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInsertValue(this);
}

void HEGADBParser::InsertValueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<HEGADBParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInsertValue(this);
}


std::any HEGADBParser::InsertValueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<HEGADBParserVisitor*>(visitor))
    return parserVisitor->visitInsertValue(this);
  else
    return visitor->visitChildren(this);
}

HEGADBParser::InsertValueContext* HEGADBParser::insertValue() {
  InsertValueContext *_localctx = _tracker.createInstance<InsertValueContext>(_ctx, getState());
  enterRule(_localctx, 20, HEGADBParser::RuleInsertValue);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(145);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::TRUE:
      case HEGADBParser::FALSE:
      case HEGADBParser::INTEGER_LITERAL:
      case HEGADBParser::STRING_LITERAL: {
        enterOuterAlt(_localctx, 1);
        setState(143);
        literal();
        break;
      }

      case HEGADBParser::NULL_: {
        enterOuterAlt(_localctx, 2);
        setState(144);
        match(HEGADBParser::NULL_);
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
  enterRule(_localctx, 22, HEGADBParser::RuleSelectList);
  size_t _la = 0;

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
      case HEGADBParser::STAR: {
        enterOuterAlt(_localctx, 1);
        setState(147);
        match(HEGADBParser::STAR);
        break;
      }

      case HEGADBParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 2);
        setState(148);
        selectItem();
        setState(153);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == HEGADBParser::COMMA) {
          setState(149);
          match(HEGADBParser::COMMA);
          setState(150);
          selectItem();
          setState(155);
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
  enterRule(_localctx, 24, HEGADBParser::RuleSelectItem);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(160);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(158);
      qualifiedColumn();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(159);
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
  enterRule(_localctx, 26, HEGADBParser::RuleJoinClause);
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
    setState(163);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == HEGADBParser::INNER) {
      setState(162);
      match(HEGADBParser::INNER);
    }
    setState(165);
    match(HEGADBParser::JOIN);
    setState(166);
    tableName();
    setState(167);
    match(HEGADBParser::ON);
    setState(168);
    qualifiedColumn();
    setState(169);
    match(HEGADBParser::EQ);
    setState(170);
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
  enterRule(_localctx, 28, HEGADBParser::RuleQualifiedColumn);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(172);
    tableName();
    setState(173);
    match(HEGADBParser::DOT);
    setState(174);
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
  enterRule(_localctx, 30, HEGADBParser::RuleTableName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(176);
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
  enterRule(_localctx, 32, HEGADBParser::RuleWhereClause);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(178);
    match(HEGADBParser::WHERE);
    setState(179);
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
  enterRule(_localctx, 34, HEGADBParser::RuleBooleanExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(181);
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
  enterRule(_localctx, 36, HEGADBParser::RuleOrExpression);
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
    setState(183);
    andExpression();
    setState(188);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HEGADBParser::OR) {
      setState(184);
      match(HEGADBParser::OR);
      setState(185);
      andExpression();
      setState(190);
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
  enterRule(_localctx, 38, HEGADBParser::RuleAndExpression);
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
    setState(191);
    notExpression();
    setState(196);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == HEGADBParser::AND) {
      setState(192);
      match(HEGADBParser::AND);
      setState(193);
      notExpression();
      setState(198);
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
  enterRule(_localctx, 40, HEGADBParser::RuleNotExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(206);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::NOT: {
        enterOuterAlt(_localctx, 1);
        setState(199);
        match(HEGADBParser::NOT);
        setState(200);
        notExpression();
        break;
      }

      case HEGADBParser::LPAREN: {
        enterOuterAlt(_localctx, 2);
        setState(201);
        match(HEGADBParser::LPAREN);
        setState(202);
        booleanExpression();
        setState(203);
        match(HEGADBParser::RPAREN);
        break;
      }

      case HEGADBParser::TRUE:
      case HEGADBParser::FALSE:
      case HEGADBParser::INTEGER_LITERAL:
      case HEGADBParser::STRING_LITERAL:
      case HEGADBParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 3);
        setState(205);
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
  enterRule(_localctx, 42, HEGADBParser::RuleComparisonExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(208);
    operand();
    setState(209);
    comparisonOperator();
    setState(210);
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
  enterRule(_localctx, 44, HEGADBParser::RuleComparisonOperator);
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
    setState(212);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 528482304) != 0))) {
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
  enterRule(_localctx, 46, HEGADBParser::RuleOperand);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(216);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case HEGADBParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(214);
        columnName();
        break;
      }

      case HEGADBParser::TRUE:
      case HEGADBParser::FALSE:
      case HEGADBParser::INTEGER_LITERAL:
      case HEGADBParser::STRING_LITERAL: {
        enterOuterAlt(_localctx, 2);
        setState(215);
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
  enterRule(_localctx, 48, HEGADBParser::RuleColumnName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(218);
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
  enterRule(_localctx, 50, HEGADBParser::RuleLiteral);
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
    setState(220);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 103079215488) != 0))) {
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
