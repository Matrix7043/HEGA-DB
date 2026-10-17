#ifndef NULL_LITERAL_H
#define NULL_LITERAL_H

#include "ASTNode.h"
#include "Expr.h"

namespace hegadb::client::ast {

// The NULL value in an INSERT row.
//
// It is a separate node rather than a Literal because Literal::Value has no
// way to hold NULL, and NULL is not a value to encrypt: it never becomes a
// query parameter.
class NullLiteral : public Expr {
public:
    ASTNodeKind kind() const override { return ASTNodeKind::NullLiteral; }
};

} // namespace hegadb::client::ast

#endif // !NULL_LITERAL_H
