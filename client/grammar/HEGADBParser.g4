parser grammar HEGADBParser;

options {
    tokenVocab = HEGADBLexer;
}


// ============================================================
// Entry point
// ============================================================

query
    : selectStatement EOF
    ;


// ============================================================
// SELECT statement
// ============================================================

selectStatement
    : SELECT
      selectList
      FROM
      tableName
      whereClause?
      SEMICOLON?
    ;


// ============================================================
// SELECT list
// ============================================================

selectList
    : STAR
    | columnName (COMMA columnName)*
    ;


// ============================================================
// Table
// ============================================================

tableName
    : IDENTIFIER
    ;


// ============================================================
// WHERE
// ============================================================

whereClause
    : WHERE booleanExpression
    ;


// ============================================================
// Boolean expressions
//
// Precedence:
//
// NOT
//  ↓
// AND
//  ↓
// OR
// ============================================================

booleanExpression
    : orExpression
    ;

orExpression
    : andExpression
      (OR andExpression)*
    ;

andExpression
    : notExpression
      (AND notExpression)*
    ;

notExpression
    : NOT notExpression
    | LPAREN booleanExpression RPAREN
    | comparisonExpression
    ;


// ============================================================
// Comparisons
// ============================================================

comparisonExpression
    : operand comparisonOperator operand
    ;

comparisonOperator
    : EQ
    | NEQ
    | LT
    | GT
    | LTE
    | GTE
    ;


// ============================================================
// Operands
// ============================================================

operand
    : columnName
    | literal
    ;


// ============================================================
// Column
// ============================================================

columnName
    : IDENTIFIER
    ;


// ============================================================
// Literals
// ============================================================

literal
    : INTEGER_LITERAL
    | STRING_LITERAL
    | TRUE
    | FALSE
    ;
