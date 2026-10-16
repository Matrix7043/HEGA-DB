
// Generated from ./HEGADBLexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  HEGADBLexer : public antlr4::Lexer {
public:
  enum {
    SELECT = 1, FROM = 2, WHERE = 3, AND = 4, OR = 5, NOT = 6, TRUE = 7, 
    FALSE = 8, NULL_ = 9, CREATE = 10, DROP = 11, TABLE = 12, INTEGER = 13, 
    FLOAT = 14, STRING = 15, BOOLEAN = 16, EQ = 17, NEQ = 18, LTE = 19, 
    GTE = 20, LT = 21, GT = 22, STAR = 23, COMMA = 24, LPAREN = 25, RPAREN = 26, 
    SEMICOLON = 27, INTEGER_LITERAL = 28, STRING_LITERAL = 29, IDENTIFIER = 30, 
    WS = 31, LINE_COMMENT = 32, BLOCK_COMMENT = 33
  };

  explicit HEGADBLexer(antlr4::CharStream *input);

  ~HEGADBLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

