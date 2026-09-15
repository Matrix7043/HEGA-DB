#ifndef SEMANTIC_ERROR_H
#define SEMANTIC_ERROR_H

#include <stdexcept>
#include <string>

namespace hegadb::client::semantic {

enum class SemanticErrorKind {
    TableNotFound,
    ColumnNotFound,
    TypeMismatch,
    AmbigousColumn,
    DuplicateColumn
};

class SemanticError : public std::runtime_error {

public:
    SemanticError(SemanticErrorKind kind, std::string message)
        : kind_(kind)
        , std::runtime_error(message)
    {
    }

    SemanticErrorKind kind() const noexcept { return kind_; }

private:
    SemanticErrorKind kind_;
};

} // namespace hegadb

#endif // SEMANTIC_ERROR_H
