#ifndef TABLE_REF_H
#define TABLE_REF_H

#include "ASTNode.h"

#include <string>
#include <utility>

namespace hegadb {

class TableRef : public ASTNode {
public:
  explicit TableRef(std::string name) : name_(std::move(name)) {}
  const std::string &name() const { return name_; }

private:
  std::string name_;
};
} // namespace hegadb

#endif
