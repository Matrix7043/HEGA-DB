#ifndef LITERAL_H
#define LITERAL_H

#include "Expr.h"

#include <string>
#include <utility>
#include <variant>

namespace hegadb {
class Literal : public Expr {
public:
  using Value = std::variant<long long, std::string, bool>;
  explicit Literal(Value value) : value_(std::move(value)) {}
  const Value &value() const { return value_; }

private:
  Value value_;
};
} // namespace hegadb

#endif
