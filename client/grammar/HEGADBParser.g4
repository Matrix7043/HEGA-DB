parser grammar HEGADBParser;

options {
    tokenVocab = HEGADBLexer;
}


// ============================================================
// Entry point
// ============================================================

query
    : statement EOF
    ;

statement
    : selectStatement
    | createTableStatement
    | dropTableStatement
    ;


// ============================================================
// SELECT statement
// ============================================================

// A SELECT has either a JOIN or a WHERE clause, never both.
selectStatement
    : SELECT
      selectList
      FROM
      tableName
      (joinClause | whereClause)?
      SEMICOLON?
    ;


// ============================================================
// CREATE TABLE statement
//
// CREATE TABLE users (id INTEGER NOT NULL, name STRING);
//
// Columns are nullable unless NOT NULL is given.
// ============================================================

createTableStatement
    : CREATE TABLE
      tableName
      LPAREN
      columnDefinition (COMMA columnDefinition)*
      RPAREN
      SEMICOLON?
    ;

columnDefinition
    : columnName dataType nullability?
    ;

nullability
    : NOT NULL_
    ;

dataType
    : INTEGER
    | FLOAT
    | STRING
    | BOOLEAN
    ;


// ============================================================
// DROP TABLE statement
//
// DROP TABLE users;
// ============================================================

dropTableStatement
    : DROP TABLE
      tableName
      SEMICOLON?
    ;


// ============================================================
// SELECT list
// ============================================================

selectList
    : STAR
    | selectItem (COMMA selectItem)*
    ;

selectItem
    : qualifiedColumn
    | columnName
    ;


// ============================================================
// INNER JOIN (equi join on a single column pair, no aliases)
//
// SELECT * FROM users INNER JOIN orders ON users.id = orders.user_id;
//
// INNER is optional: JOIN means INNER JOIN.
// ============================================================

joinClause
    : INNER? JOIN
      tableName
      ON
      qualifiedColumn EQ qualifiedColumn
    ;

qualifiedColumn
    : tableName DOT columnName
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
