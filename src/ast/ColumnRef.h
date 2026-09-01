#ifndef COLUMN_REF_H
#define COLUMN_REF_H

#include "Expr.h"

#include <string>
#include <utility>

namespace hegadb {

class ColumnRef : public Expr {
public:
  explicit ColumnRef(std::string name) : name_(std::move(name)) {}
  const std::string &name() const { return name_; }

private:
  std::string name_;
};
} // namespace hegadb

#endif
