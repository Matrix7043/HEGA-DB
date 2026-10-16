
// Generated from ./HEGADBLexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  HEGADBLexer : public antlr4::Lexer {
public:
  enum {
    SELECT = 1, FROM = 2, WHERE = 3, AND = 4, OR = 5, NOT = 6, TRUE = 7, 
    FALSE = 8, NULL_ = 9, CREATE = 10, DROP = 11, TABLE = 12, INNER = 13, 
    JOIN = 14, ON = 15, INTEGER = 16, FLOAT = 17, STRING = 18, BOOLEAN = 19, 
    EQ = 20, NEQ = 21, LTE = 22, GTE = 23, LT = 24, GT = 25, STAR = 26, 
    COMMA = 27, DOT = 28, LPAREN = 29, RPAREN = 30, SEMICOLON = 31, INTEGER_LITERAL = 32, 
    STRING_LITERAL = 33, IDENTIFIER = 34, WS = 35, LINE_COMMENT = 36, BLOCK_COMMENT = 37
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

