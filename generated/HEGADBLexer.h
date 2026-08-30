
// Generated from ./HEGADBLexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  HEGADBLexer : public antlr4::Lexer {
public:
  enum {
    SELECT = 1, FROM = 2, WHERE = 3, AND = 4, OR = 5, NOT = 6, TRUE = 7, 
    FALSE = 8, EQ = 9, NEQ = 10, LTE = 11, GTE = 12, LT = 13, GT = 14, STAR = 15, 
    COMMA = 16, LPAREN = 17, RPAREN = 18, SEMICOLON = 19, INTEGER_LITERAL = 20, 
    STRING_LITERAL = 21, IDENTIFIER = 22, WS = 23, LINE_COMMENT = 24, BLOCK_COMMENT = 25
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

