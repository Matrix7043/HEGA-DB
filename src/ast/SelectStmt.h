#ifndef SELECT_STMT_H
#define SELECT_STMT_H

#include "ColumnRef.h"
#include "Statement.h"
#include "TableRef.h"

#include <memory>
#include <vector>

namespace hegadb {

class SelectStmt : public Statement {
public:
  void addProjection(std::unique_ptr<ColumnRef> column) {
    projections_.push_back(std::move(column));
  }
  void setFrom(std::unique_ptr<TableRef> table) { from_ = std::move(table); }
  const std::vector<std::unique_ptr<ColumnRef>> &projections() const {
    return projections_;
  }
  const TableRef *from() const { return from_.get(); }

private:
  std::vector<std::unique_ptr<ColumnRef>> projections_;
  std::unique_ptr<TableRef> from_;
};
} // namespace hegadb

#endif
