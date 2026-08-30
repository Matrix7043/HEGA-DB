
// Generated from ./HEGADBLexer.g4 by ANTLR 4.13.2


#include "HEGADBLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct HEGADBLexerStaticData final {
  HEGADBLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  HEGADBLexerStaticData(const HEGADBLexerStaticData&) = delete;
  HEGADBLexerStaticData(HEGADBLexerStaticData&&) = delete;
  HEGADBLexerStaticData& operator=(const HEGADBLexerStaticData&) = delete;
  HEGADBLexerStaticData& operator=(HEGADBLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag hegadblexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<HEGADBLexerStaticData> hegadblexerLexerStaticData = nullptr;

void hegadblexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (hegadblexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(hegadblexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<HEGADBLexerStaticData>(
    std::vector<std::string>{
      "SELECT", "FROM", "WHERE", "AND", "OR", "NOT", "TRUE", "FALSE", "EQ", 
      "NEQ", "LTE", "GTE", "LT", "GT", "STAR", "COMMA", "LPAREN", "RPAREN", 
      "SEMICOLON", "INTEGER_LITERAL", "STRING_LITERAL", "IDENTIFIER", "WS", 
      "LINE_COMMENT", "BLOCK_COMMENT", "A", "B", "C", "D", "E", "F", "G", 
      "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", 
      "V", "W", "X", "Y", "Z"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
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
  	4,0,25,275,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,
  	7,21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,
  	7,28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,
  	7,35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,
  	7,42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,
  	7,49,2,50,7,50,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,2,1,
  	2,1,2,1,2,1,2,1,2,1,3,1,3,1,3,1,3,1,4,1,4,1,4,1,5,1,5,1,5,1,5,1,6,1,6,
  	1,6,1,6,1,6,1,7,1,7,1,7,1,7,1,7,1,7,1,8,1,8,1,9,1,9,1,9,1,10,1,10,1,10,
  	1,11,1,11,1,11,1,12,1,12,1,13,1,13,1,14,1,14,1,15,1,15,1,16,1,16,1,17,
  	1,17,1,18,1,18,1,19,4,19,170,8,19,11,19,12,19,171,1,20,1,20,1,20,1,20,
  	5,20,178,8,20,10,20,12,20,181,9,20,1,20,1,20,1,21,1,21,5,21,187,8,21,
  	10,21,12,21,190,9,21,1,22,4,22,193,8,22,11,22,12,22,194,1,22,1,22,1,23,
  	1,23,1,23,1,23,5,23,203,8,23,10,23,12,23,206,9,23,1,23,1,23,1,24,1,24,
  	1,24,1,24,5,24,214,8,24,10,24,12,24,217,9,24,1,24,1,24,1,24,1,24,1,24,
  	1,25,1,25,1,26,1,26,1,27,1,27,1,28,1,28,1,29,1,29,1,30,1,30,1,31,1,31,
  	1,32,1,32,1,33,1,33,1,34,1,34,1,35,1,35,1,36,1,36,1,37,1,37,1,38,1,38,
  	1,39,1,39,1,40,1,40,1,41,1,41,1,42,1,42,1,43,1,43,1,44,1,44,1,45,1,45,
  	1,46,1,46,1,47,1,47,1,48,1,48,1,49,1,49,1,50,1,50,1,215,0,51,1,1,3,2,
  	5,3,7,4,9,5,11,6,13,7,15,8,17,9,19,10,21,11,23,12,25,13,27,14,29,15,31,
  	16,33,17,35,18,37,19,39,20,41,21,43,22,45,23,47,24,49,25,51,0,53,0,55,
  	0,57,0,59,0,61,0,63,0,65,0,67,0,69,0,71,0,73,0,75,0,77,0,79,0,81,0,83,
  	0,85,0,87,0,89,0,91,0,93,0,95,0,97,0,99,0,101,0,1,0,32,1,0,48,57,1,0,
  	39,39,3,0,65,90,95,95,97,122,4,0,48,57,65,90,95,95,97,122,3,0,9,10,13,
  	13,32,32,2,0,10,10,13,13,2,0,65,65,97,97,2,0,66,66,98,98,2,0,67,67,99,
  	99,2,0,68,68,100,100,2,0,69,69,101,101,2,0,70,70,102,102,2,0,71,71,103,
  	103,2,0,72,72,104,104,2,0,73,73,105,105,2,0,74,74,106,106,2,0,75,75,107,
  	107,2,0,76,76,108,108,2,0,77,77,109,109,2,0,78,78,110,110,2,0,79,79,111,
  	111,2,0,80,80,112,112,2,0,81,81,113,113,2,0,82,82,114,114,2,0,83,83,115,
  	115,2,0,84,84,116,116,2,0,85,85,117,117,2,0,86,86,118,118,2,0,87,87,119,
  	119,2,0,88,88,120,120,2,0,89,89,121,121,2,0,90,90,122,122,255,0,1,1,0,
  	0,0,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,9,1,0,0,0,0,11,1,0,0,0,0,13,
  	1,0,0,0,0,15,1,0,0,0,0,17,1,0,0,0,0,19,1,0,0,0,0,21,1,0,0,0,0,23,1,0,
  	0,0,0,25,1,0,0,0,0,27,1,0,0,0,0,29,1,0,0,0,0,31,1,0,0,0,0,33,1,0,0,0,
  	0,35,1,0,0,0,0,37,1,0,0,0,0,39,1,0,0,0,0,41,1,0,0,0,0,43,1,0,0,0,0,45,
  	1,0,0,0,0,47,1,0,0,0,0,49,1,0,0,0,1,103,1,0,0,0,3,110,1,0,0,0,5,115,1,
  	0,0,0,7,121,1,0,0,0,9,125,1,0,0,0,11,128,1,0,0,0,13,132,1,0,0,0,15,137,
  	1,0,0,0,17,143,1,0,0,0,19,145,1,0,0,0,21,148,1,0,0,0,23,151,1,0,0,0,25,
  	154,1,0,0,0,27,156,1,0,0,0,29,158,1,0,0,0,31,160,1,0,0,0,33,162,1,0,0,
  	0,35,164,1,0,0,0,37,166,1,0,0,0,39,169,1,0,0,0,41,173,1,0,0,0,43,184,
  	1,0,0,0,45,192,1,0,0,0,47,198,1,0,0,0,49,209,1,0,0,0,51,223,1,0,0,0,53,
  	225,1,0,0,0,55,227,1,0,0,0,57,229,1,0,0,0,59,231,1,0,0,0,61,233,1,0,0,
  	0,63,235,1,0,0,0,65,237,1,0,0,0,67,239,1,0,0,0,69,241,1,0,0,0,71,243,
  	1,0,0,0,73,245,1,0,0,0,75,247,1,0,0,0,77,249,1,0,0,0,79,251,1,0,0,0,81,
  	253,1,0,0,0,83,255,1,0,0,0,85,257,1,0,0,0,87,259,1,0,0,0,89,261,1,0,0,
  	0,91,263,1,0,0,0,93,265,1,0,0,0,95,267,1,0,0,0,97,269,1,0,0,0,99,271,
  	1,0,0,0,101,273,1,0,0,0,103,104,3,87,43,0,104,105,3,59,29,0,105,106,3,
  	73,36,0,106,107,3,59,29,0,107,108,3,55,27,0,108,109,3,89,44,0,109,2,1,
  	0,0,0,110,111,3,61,30,0,111,112,3,85,42,0,112,113,3,79,39,0,113,114,3,
  	75,37,0,114,4,1,0,0,0,115,116,3,95,47,0,116,117,3,65,32,0,117,118,3,59,
  	29,0,118,119,3,85,42,0,119,120,3,59,29,0,120,6,1,0,0,0,121,122,3,51,25,
  	0,122,123,3,77,38,0,123,124,3,57,28,0,124,8,1,0,0,0,125,126,3,79,39,0,
  	126,127,3,85,42,0,127,10,1,0,0,0,128,129,3,77,38,0,129,130,3,79,39,0,
  	130,131,3,89,44,0,131,12,1,0,0,0,132,133,3,89,44,0,133,134,3,85,42,0,
  	134,135,3,91,45,0,135,136,3,59,29,0,136,14,1,0,0,0,137,138,3,61,30,0,
  	138,139,3,51,25,0,139,140,3,73,36,0,140,141,3,87,43,0,141,142,3,59,29,
  	0,142,16,1,0,0,0,143,144,5,61,0,0,144,18,1,0,0,0,145,146,5,33,0,0,146,
  	147,5,61,0,0,147,20,1,0,0,0,148,149,5,60,0,0,149,150,5,61,0,0,150,22,
  	1,0,0,0,151,152,5,62,0,0,152,153,5,61,0,0,153,24,1,0,0,0,154,155,5,60,
  	0,0,155,26,1,0,0,0,156,157,5,62,0,0,157,28,1,0,0,0,158,159,5,42,0,0,159,
  	30,1,0,0,0,160,161,5,44,0,0,161,32,1,0,0,0,162,163,5,40,0,0,163,34,1,
  	0,0,0,164,165,5,41,0,0,165,36,1,0,0,0,166,167,5,59,0,0,167,38,1,0,0,0,
  	168,170,7,0,0,0,169,168,1,0,0,0,170,171,1,0,0,0,171,169,1,0,0,0,171,172,
  	1,0,0,0,172,40,1,0,0,0,173,179,5,39,0,0,174,175,5,39,0,0,175,178,5,39,
  	0,0,176,178,8,1,0,0,177,174,1,0,0,0,177,176,1,0,0,0,178,181,1,0,0,0,179,
  	177,1,0,0,0,179,180,1,0,0,0,180,182,1,0,0,0,181,179,1,0,0,0,182,183,5,
  	39,0,0,183,42,1,0,0,0,184,188,7,2,0,0,185,187,7,3,0,0,186,185,1,0,0,0,
  	187,190,1,0,0,0,188,186,1,0,0,0,188,189,1,0,0,0,189,44,1,0,0,0,190,188,
  	1,0,0,0,191,193,7,4,0,0,192,191,1,0,0,0,193,194,1,0,0,0,194,192,1,0,0,
  	0,194,195,1,0,0,0,195,196,1,0,0,0,196,197,6,22,0,0,197,46,1,0,0,0,198,
  	199,5,45,0,0,199,200,5,45,0,0,200,204,1,0,0,0,201,203,8,5,0,0,202,201,
  	1,0,0,0,203,206,1,0,0,0,204,202,1,0,0,0,204,205,1,0,0,0,205,207,1,0,0,
  	0,206,204,1,0,0,0,207,208,6,23,0,0,208,48,1,0,0,0,209,210,5,47,0,0,210,
  	211,5,42,0,0,211,215,1,0,0,0,212,214,9,0,0,0,213,212,1,0,0,0,214,217,
  	1,0,0,0,215,216,1,0,0,0,215,213,1,0,0,0,216,218,1,0,0,0,217,215,1,0,0,
  	0,218,219,5,42,0,0,219,220,5,47,0,0,220,221,1,0,0,0,221,222,6,24,0,0,
  	222,50,1,0,0,0,223,224,7,6,0,0,224,52,1,0,0,0,225,226,7,7,0,0,226,54,
  	1,0,0,0,227,228,7,8,0,0,228,56,1,0,0,0,229,230,7,9,0,0,230,58,1,0,0,0,
  	231,232,7,10,0,0,232,60,1,0,0,0,233,234,7,11,0,0,234,62,1,0,0,0,235,236,
  	7,12,0,0,236,64,1,0,0,0,237,238,7,13,0,0,238,66,1,0,0,0,239,240,7,14,
  	0,0,240,68,1,0,0,0,241,242,7,15,0,0,242,70,1,0,0,0,243,244,7,16,0,0,244,
  	72,1,0,0,0,245,246,7,17,0,0,246,74,1,0,0,0,247,248,7,18,0,0,248,76,1,
  	0,0,0,249,250,7,19,0,0,250,78,1,0,0,0,251,252,7,20,0,0,252,80,1,0,0,0,
  	253,254,7,21,0,0,254,82,1,0,0,0,255,256,7,22,0,0,256,84,1,0,0,0,257,258,
  	7,23,0,0,258,86,1,0,0,0,259,260,7,24,0,0,260,88,1,0,0,0,261,262,7,25,
  	0,0,262,90,1,0,0,0,263,264,7,26,0,0,264,92,1,0,0,0,265,266,7,27,0,0,266,
  	94,1,0,0,0,267,268,7,28,0,0,268,96,1,0,0,0,269,270,7,29,0,0,270,98,1,
  	0,0,0,271,272,7,30,0,0,272,100,1,0,0,0,273,274,7,31,0,0,274,102,1,0,0,
  	0,8,0,171,177,179,188,194,204,215,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  hegadblexerLexerStaticData = std::move(staticData);
}

}

HEGADBLexer::HEGADBLexer(CharStream *input) : Lexer(input) {
  HEGADBLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *hegadblexerLexerStaticData->atn, hegadblexerLexerStaticData->decisionToDFA, hegadblexerLexerStaticData->sharedContextCache);
}

HEGADBLexer::~HEGADBLexer() {
  delete _interpreter;
}

std::string HEGADBLexer::getGrammarFileName() const {
  return "HEGADBLexer.g4";
}

const std::vector<std::string>& HEGADBLexer::getRuleNames() const {
  return hegadblexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& HEGADBLexer::getChannelNames() const {
  return hegadblexerLexerStaticData->channelNames;
}

const std::vector<std::string>& HEGADBLexer::getModeNames() const {
  return hegadblexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& HEGADBLexer::getVocabulary() const {
  return hegadblexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView HEGADBLexer::getSerializedATN() const {
  return hegadblexerLexerStaticData->serializedATN;
}

const atn::ATN& HEGADBLexer::getATN() const {
  return *hegadblexerLexerStaticData->atn;
}




void HEGADBLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  hegadblexerLexerInitialize();
#else
  ::antlr4::internal::call_once(hegadblexerLexerOnceFlag, hegadblexerLexerInitialize);
#endif
}
