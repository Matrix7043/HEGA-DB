lexer grammar HEGADBLexer;

// ============================================================
// Keywords
// ============================================================

SELECT
    : S E L E C T
    ;

FROM
    : F R O M
    ;

WHERE
    : W H E R E
    ;

AND
    : A N D
    ;

OR
    : O R
    ;

NOT
    : N O T
    ;

TRUE
    : T R U E
    ;

FALSE
    : F A L S E
    ;

// Named NULL_ because NULL is a macro in C/C++ and would break
// the generated lexer/parser enum.
NULL_
    : N U L L
    ;


// ============================================================
// DDL keywords
// ============================================================

CREATE
    : C R E A T E
    ;

DROP
    : D R O P
    ;

TABLE
    : T A B L E
    ;


// ============================================================
// Data type keywords
//
// Mirrors hegadb::types::DataType. No new types are
// introduced here.
// ============================================================

INTEGER
    : I N T E G E R
    ;

FLOAT
    : F L O A T
    ;

STRING
    : S T R I N G
    ;

BOOLEAN
    : B O O L E A N
    ;


// ============================================================
// Comparison operators
// ============================================================

EQ
    : '='
    ;

NEQ
    : '!='
    ;

LTE
    : '<='
    ;

GTE
    : '>='
    ;

LT
    : '<'
    ;

GT
    : '>'
    ;


// ============================================================
// Punctuation
// ============================================================

STAR
    : '*'
    ;

COMMA
    : ','
    ;

LPAREN
    : '('
    ;

RPAREN
    : ')'
    ;

SEMICOLON
    : ';'
    ;


// ============================================================
// Literals
// ============================================================

INTEGER_LITERAL
    : [0-9]+
    ;

STRING_LITERAL
    : '\'' ( '\'\'' | ~'\'' )* '\''
    ;


// ============================================================
// Identifiers
// ============================================================

IDENTIFIER
    : [a-zA-Z_] [a-zA-Z0-9_]*
    ;


// ============================================================
// Whitespace
// ============================================================

WS
    : [ \t\r\n]+ -> skip
    ;


// ============================================================
// Comments
// ============================================================

LINE_COMMENT
    : '--' ~[\r\n]* -> skip
    ;

BLOCK_COMMENT
    : '/*' .*? '*/' -> skip
    ;


// ============================================================
// Case-insensitive keyword fragments
// ============================================================

fragment A : [aA];
fragment B : [bB];
fragment C : [cC];
fragment D : [dD];
fragment E : [eE];
fragment F : [fF];
fragment G : [gG];
fragment H : [hH];
fragment I : [iI];
fragment J : [jJ];
fragment K : [kK];
fragment L : [lL];
fragment M : [mM];
fragment N : [nN];
fragment O : [oO];
fragment P : [pP];
fragment Q : [qQ];
fragment R : [rR];
fragment S : [sS];
fragment T : [tT];
fragment U : [uU];
fragment V : [vV];
fragment W : [wW];
fragment X : [xX];
fragment Y : [yY];
fragment Z : [zZ];

