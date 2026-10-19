#include "hegadb/client/semantic/SemanticValidator.h"
#include "hegadb/catalog/TableSchema.h"
#include "hegadb/client/semantic/SemanticError.h"
#include "hegadb/types/DataType.h"

#include "hegadb/client/ast/ColumnRef.h"
#include "hegadb/client/ast/ComparisonExpr.h"
#include "hegadb/client/ast/Literal.h"
#include "hegadb/client/ast/NullLiteral.h"
#include "hegadb/client/ast/LogicalExpr.h"
#include "hegadb/client/ast/NotExpr.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

namespace hegadb::client::semantic {

namespace {

struct ResolvedColumn {
    catalog::TableSchema const* table;
    catalog::ColumnSchema const* column;
};

std::string describeTables(catalog::TableSchema const& left,
    catalog::TableSchema const& right)
{
    return "'" + left.name() + "' or '" + right.name() + "'";
}

// Resolves a table-qualified column (table.column) against the two tables
// of a join.
ResolvedColumn resolveQualified(ast::ColumnRef const& column,
    catalog::TableSchema const& left,
    catalog::TableSchema const& right)
{
    std::string const& qualifier = *column.table();

    catalog::TableSchema const* table = nullptr;

    if (qualifier == left.name()) {
        table = &left;
    } else if (qualifier == right.name()) {
        table = &right;
    } else {
        throw SemanticError(SemanticErrorKind::TableNotFound,
            "table '" + qualifier + "' is not part of this query (expected "
                + describeTables(left, right) + ")");
    }

    catalog::ColumnSchema const* found = table->findColumn(column.name());

    if (found == nullptr) {
        throw SemanticError(SemanticErrorKind::ColumnNotFound,
            "column '" + column.name() + "' does not exist in table '" + table->name() + "'");
    }

    return { table, found };
}

const char* typeName(types::DataType type)
{
    switch (type) {
    case types::DataType::Integer:
        return "INTEGER";
    case types::DataType::Float:
        return "FLOAT";
    case types::DataType::String:
        return "STRING";
    case types::DataType::Boolean:
        return "BOOLEAN";
    }

    return "UNKNOWN";
}

const char* literalTypeName(ast::Literal::Value const& value)
{
    if (std::holds_alternative<long long>(value)) {
        return "INTEGER";
    }

    if (std::holds_alternative<std::string>(value)) {
        return "STRING";
    }

    return "BOOLEAN";
}

bool literalMatches(ast::Literal::Value const& value, types::DataType type)
{
    switch (type) {
    case types::DataType::Integer:
        return std::holds_alternative<long long>(value);
    case types::DataType::String:
        return std::holds_alternative<std::string>(value);
    case types::DataType::Boolean:
        return std::holds_alternative<bool>(value);
    case types::DataType::Float:
        // The language has no float literal, and an integer must not be
        // widened: it would be encrypted with the integer scheme.
        return false;
    }

    return false;
}

} // namespace

SemanticValidator::SemanticValidator(catalog::Catalog const& catalog)
    : catalog_(catalog)
{
}

void SemanticValidator::validate(ast::Statement const& stmt) const
{
    switch (stmt.kind()) {

    case ast::ASTNodeKind::SelectStmt:
        validate(static_cast<ast::SelectStmt const&>(stmt));
        return;

    case ast::ASTNodeKind::CreateTableStmt:
        validate(static_cast<ast::CreateTableStmt const&>(stmt));
        return;

    case ast::ASTNodeKind::DropTableStmt:
        validate(static_cast<ast::DropTableStmt const&>(stmt));
        return;

    case ast::ASTNodeKind::InsertStmt:
        validate(static_cast<ast::InsertStmt const&>(stmt));
        return;

    default:
        throw std::invalid_argument(
            "SemanticValidator: unsupported statement kind");
    }
}

void SemanticValidator::validate(ast::CreateTableStmt const& stmt) const
{
    std::string const& table_name = stmt.tableName();

    if (table_name.empty()) {
        throw SemanticError(SemanticErrorKind::InvalidTableDefinition,
            "table name must not be empty");
    }

    if (catalog_.findTable(table_name) != nullptr) {
        throw SemanticError(SemanticErrorKind::TableAlreadyExists,
            "table '" + table_name + "' already exists");
    }

    if (stmt.columns().empty()) {
        throw SemanticError(SemanticErrorKind::InvalidTableDefinition,
            "table '" + table_name + "' must have at least one column");
    }

    std::unordered_set<std::string> seen;

    for (auto const& column : stmt.columns()) {

        if (column.name.empty()) {
            throw SemanticError(SemanticErrorKind::InvalidTableDefinition,
                "column name must not be empty in table '" + table_name + "'");
        }

        if (!seen.insert(column.name).second) {
            throw SemanticError(SemanticErrorKind::DuplicateColumn,
                "duplicate column '" + column.name + "' in table '" + table_name + "'");
        }

        switch (column.type) {
        case types::DataType::Integer:
        case types::DataType::Float:
        case types::DataType::String:
        case types::DataType::Boolean:
            break;

        default:
            throw SemanticError(SemanticErrorKind::InvalidTableDefinition,
                "column '" + column.name + "' has an unsupported data type");
        }
    }
}

void SemanticValidator::validate(ast::DropTableStmt const& stmt) const
{
    if (catalog_.findTable(stmt.tableName()) == nullptr) {
        throw SemanticError(SemanticErrorKind::TableNotFound,
            "table '" + stmt.tableName() + "' does not exist");
    }
}

void SemanticValidator::validate(ast::InsertStmt const& stmt) const
{
    catalog::TableSchema const* table = catalog_.findTable(stmt.tableName());

    if (table == nullptr) {
        throw SemanticError(SemanticErrorKind::TableNotFound,
            "table '" + stmt.tableName() + "' does not exist");
    }

    // ---- Target columns, in the order the values are given ----

    std::vector<catalog::ColumnSchema const*> targets;

    if (stmt.hasColumnList()) {
        std::unordered_set<std::string> listed;

        for (auto const& name : stmt.columns()) {
            catalog::ColumnSchema const* column = table->findColumn(name);

            if (column == nullptr) {
                throw SemanticError(SemanticErrorKind::ColumnNotFound,
                    "column '" + name + "' does not exist in table '" + table->name() + "'");
            }

            if (!listed.insert(name).second) {
                throw SemanticError(SemanticErrorKind::DuplicateColumn,
                    "column '" + name + "' is listed more than once");
            }

            targets.push_back(column);
        }

        // A column left out of the list gets NULL.
        for (auto const& column : table->columns()) {
            if (listed.count(column.name) == 0 && !column.nullable) {
                throw SemanticError(SemanticErrorKind::NullConstraintViolation,
                    "column '" + column.name + "' is NOT NULL and has no value");
            }
        }
    } else {
        for (auto const& column : table->columns()) {
            targets.push_back(&column);
        }

        std::sort(targets.begin(), targets.end(),
            [](catalog::ColumnSchema const* a, catalog::ColumnSchema const* b) {
                return a->ordinal < b->ordinal;
            });
    }

    // ---- Rows ----

    if (stmt.rows().empty()) {
        throw SemanticError(SemanticErrorKind::ValueCountMismatch,
            "INSERT has no rows");
    }

    std::size_t row_number = 0;

    for (auto const& row : stmt.rows()) {
        ++row_number;

        if (row.size() != targets.size()) {
            throw SemanticError(SemanticErrorKind::ValueCountMismatch,
                "row " + std::to_string(row_number) + " has " + std::to_string(row.size())
                    + " value(s) but " + std::to_string(targets.size()) + " were expected");
        }

        for (std::size_t i = 0; i < row.size(); ++i) {
            catalog::ColumnSchema const& column = *targets[i];
            ast::Expr const* cell = row[i].get();

            if (cell == nullptr) {
                throw SemanticError(SemanticErrorKind::TypeMismatch,
                    "row " + std::to_string(row_number) + " has a missing value");
            }

            if (cell->kind() == ast::ASTNodeKind::NullLiteral) {
                if (!column.nullable) {
                    throw SemanticError(SemanticErrorKind::NullConstraintViolation,
                        "column '" + column.name + "' is NOT NULL but row "
                            + std::to_string(row_number) + " gives NULL");
                }
                continue;
            }

            if (cell->kind() != ast::ASTNodeKind::Literal) {
                throw SemanticError(SemanticErrorKind::TypeMismatch,
                    "INSERT values must be literals or NULL");
            }

            auto const& value = static_cast<ast::Literal const&>(*cell).value();

            if (!literalMatches(value, column.type)) {
                std::string message = "column '" + column.name + "' is "
                    + typeName(column.type) + " but row " + std::to_string(row_number)
                    + " gives a " + literalTypeName(value);

                if (column.type == types::DataType::Float) {
                    message += " (FLOAT columns accept only NULL: there are no float literals yet)";
                }

                throw SemanticError(SemanticErrorKind::TypeMismatch, message);
            }
        }
    }
}

void SemanticValidator::validate(ast::SelectStmt const& stmt) const
{

    ast::TableRef const* from = stmt.from();

    if (from == nullptr) {
        throw SemanticError(SemanticErrorKind::TableNotFound,
            "SELECT statement has no FROM table");
    }

    catalog::TableSchema const* table = catalog_.findTable(from->name());

    if (table == nullptr) {
        throw SemanticError(SemanticErrorKind::TableNotFound,
            "table '" + from->name() + "' does not exist");
    }

    if (stmt.join() != nullptr) {
        validateJoin(stmt, *table);
        return;
    }

    for (auto const& projection : stmt.projections()) {
        validateProjection(projection.get(), *table);
    }

    if (stmt.where() != nullptr) {

        validateExpression(stmt.where(), *table);

        types::DataType const whereType = inferType(stmt.where(), *table);

        if (whereType != types::DataType::Boolean) {
            throw SemanticError(SemanticErrorKind::TypeMismatch,
                "WHERE expression must evaluate to Boolean");
        }
    }
}

void SemanticValidator::validateProjection(ast::Expr const* expr,
    catalog::TableSchema const& table) const
{

    if (expr == nullptr) {
        return;
    }

    if (expr->kind() == ast::ASTNodeKind::Wildcard) {
        return;
    }

    if (expr->kind() == ast::ASTNodeKind::ColumnRef) {

        auto const* column = static_cast<ast::ColumnRef const*>(expr);

        if (column->table() && *column->table() != table.name()) {
            throw SemanticError(SemanticErrorKind::TableNotFound,
                "table '" + *column->table() + "' is not part of this query (expected '" + table.name() + "')");
        }

        if (table.findColumn(column->name()) == nullptr) {
            throw SemanticError(SemanticErrorKind::ColumnNotFound,
                "column '" + column->name() + "' does not exist in table '" + table.name() + "'");
        }

        return;
    }

    throw SemanticError(SemanticErrorKind::TypeMismatch,
        "invalid expression in SELECT projection");
}

void SemanticValidator::validateJoin(ast::SelectStmt const& stmt,
    catalog::TableSchema const& left_table) const
{
    ast::JoinClause const* join = stmt.join();

    if (stmt.where() != nullptr) {
        throw SemanticError(SemanticErrorKind::InvalidJoin,
            "WHERE cannot be combined with JOIN");
    }

    if (join->table() == nullptr || join->left() == nullptr || join->right() == nullptr) {
        throw SemanticError(SemanticErrorKind::InvalidJoin,
            "incomplete JOIN clause");
    }

    // ---- Tables ----

    catalog::TableSchema const* right_table = catalog_.findTable(join->table()->name());

    if (right_table == nullptr) {
        throw SemanticError(SemanticErrorKind::TableNotFound,
            "table '" + join->table()->name() + "' does not exist");
    }

    // Without table aliases a table cannot be told apart from itself.
    if (right_table->name() == left_table.name()) {
        throw SemanticError(SemanticErrorKind::InvalidJoin,
            "cannot join table '" + left_table.name() + "' with itself (table aliases are not supported)");
    }

    // ---- ON <left> = <right> ----

    for (ast::ColumnRef const* on_column : { join->left(), join->right() }) {
        if (!on_column->table()) {
            throw SemanticError(SemanticErrorKind::InvalidJoin,
                "join column '" + on_column->name() + "' must be qualified with a table name");
        }
    }

    ResolvedColumn const left = resolveQualified(*join->left(), left_table, *right_table);
    ResolvedColumn const right = resolveQualified(*join->right(), left_table, *right_table);

    if (left.table == right.table) {
        throw SemanticError(SemanticErrorKind::InvalidJoin,
            "JOIN condition must compare a column of '" + left_table.name() + "' with a column of '" + right_table->name() + "'");
    }

    if (left.column->type != right.column->type) {
        throw SemanticError(SemanticErrorKind::TypeMismatch,
            "join columns '" + left.table->name() + "." + left.column->name + "' and '" + right.table->name() + "." + right.column->name + "' must have the same type");
    }

    // ---- Projections ----

    for (auto const& projection : stmt.projections()) {

        if (projection == nullptr || projection->kind() == ast::ASTNodeKind::Wildcard) {
            continue;
        }

        if (projection->kind() != ast::ASTNodeKind::ColumnRef) {
            throw SemanticError(SemanticErrorKind::TypeMismatch,
                "invalid expression in SELECT projection");
        }

        auto const& column = static_cast<ast::ColumnRef const&>(*projection);

        if (column.table()) {
            resolveQualified(column, left_table, *right_table);
            continue;
        }

        bool const in_left = left_table.findColumn(column.name()) != nullptr;
        bool const in_right = right_table->findColumn(column.name()) != nullptr;

        if (in_left && in_right) {
            throw SemanticError(SemanticErrorKind::AmbigousColumn,
                "column '" + column.name() + "' is ambiguous: it exists in both '" + left_table.name() + "' and '" + right_table->name() + "' (qualify it, e.g. '" + left_table.name() + "." + column.name() + "')");
        }

        if (!in_left && !in_right) {
            throw SemanticError(SemanticErrorKind::ColumnNotFound,
                "column '" + column.name() + "' does not exist in table " + describeTables(left_table, *right_table));
        }
    }
}

void SemanticValidator::validateExpression(ast::Expr const* expr,
    catalog::TableSchema const& table) const
{

    if (expr == nullptr) {
        return;
    }

    switch (expr->kind()) {

    case ast::ASTNodeKind::ColumnRef: {
        inferType(expr, table);
        break;
    }

    case ast::ASTNodeKind::Literal: {
        inferType(expr, table);
        break;
    }

    case ast::ASTNodeKind::ComparisonExpr: {
        auto const* comparison = static_cast<ast::ComparisonExpr const*>(expr);

        validateComparison(comparison, table);
        break;
    }

    case ast::ASTNodeKind::LogicalExpr: {
        auto const* logical = static_cast<ast::LogicalExpr const*>(expr);

        validateExpression(logical->left(), table);
        validateExpression(logical->right(), table);

        types::DataType const leftType = inferType(logical->left(), table);

        types::DataType const rightType = inferType(logical->right(), table);

        if (leftType != types::DataType::Boolean || rightType != types::DataType::Boolean) {

            throw SemanticError(SemanticErrorKind::TypeMismatch,
                "logical operators require boolean expressions");
        }

        break;
    }

    case ast::ASTNodeKind::NotExpr: {
        auto const* notExpr = static_cast<ast::NotExpr const*>(expr);

        validateExpression(notExpr->child(), table);

        types::DataType const childType = inferType(notExpr->child(), table);

        if (childType != types::DataType::Boolean) {
            throw SemanticError(SemanticErrorKind::TypeMismatch,
                "NOT requires a boolean expression");
        }

        break;
    }

    default:
        throw SemanticError(SemanticErrorKind::TypeMismatch,
            "invalid expression in WHERE clause");
    }
}

types::DataType SemanticValidator::inferType(ast::Expr const* expr,
    catalog::TableSchema const& table) const
{

    if (expr == nullptr) {
        throw std::invalid_argument("Cannot infer type of null expression");
    }

    if (expr->kind() == ast::ASTNodeKind::ColumnRef) {

        auto const* column = static_cast<ast::ColumnRef const*>(expr);

        catalog::ColumnSchema const* schema = table.findColumn(column->name());

        if (schema == nullptr) {
            throw SemanticError(SemanticErrorKind::ColumnNotFound,
                "column '" + column->name() + "' does not exist in table '" + table.name() + "'");
        }

        return schema->type;
    }

    if (expr->kind() == ast::ASTNodeKind::Literal) {

        auto const* literal = static_cast<ast::Literal const*>(expr);

        auto const& value = literal->value();

        if (std::holds_alternative<long long>(value)) {
            return types::DataType::Integer;
        }

        if (std::holds_alternative<std::string>(value)) {
            return types::DataType::String;
        }

        if (std::holds_alternative<bool>(value)) {
            return types::DataType::Boolean;
        }
    }

    if (expr->kind() == ast::ASTNodeKind::ComparisonExpr) {

        auto const* comparison = static_cast<ast::ComparisonExpr const*>(expr);
        types::DataType const leftType = inferType(comparison->left(), table);
        types::DataType const rightType = inferType(comparison->right(), table);
        if (leftType != rightType) {
            throw SemanticError(SemanticErrorKind::TypeMismatch,
                "comparison operands must have compatible types");
        }
        return types::DataType::Boolean;
    }

    if (expr->kind() == ast::ASTNodeKind::LogicalExpr) {

        auto const* logical = static_cast<ast::LogicalExpr const*>(expr);
        types::DataType const leftType = inferType(logical->left(), table);
        types::DataType const rightType = inferType(logical->right(), table);
        if (leftType != types::DataType::Boolean || rightType != types::DataType::Boolean) {
            throw SemanticError(SemanticErrorKind::TypeMismatch,
                "logical operands must have compatible types");
        }
        return types::DataType::Boolean;
    }

    if (expr->kind() == ast::ASTNodeKind::NotExpr) {
        auto const* notExpr = static_cast<ast::NotExpr const*>(expr);
        types::DataType const childType = inferType(notExpr->child(), table);
        if (childType != types::DataType::Boolean) {
            throw SemanticError(SemanticErrorKind::TypeMismatch,
                "NOT operand must be Boolean");
        }
        return types::DataType::Boolean;
    }

    throw std::runtime_error(
        "Type inference not implemented for this expression");
}

bool SemanticValidator::areComparable(types::DataType left, types::DataType right)
{

    if (left == right) {
        return true;
    }

    if (((left == types::DataType::Integer) && (right == types::DataType::Float)) || ((left == types::DataType::Float) && (right == types::DataType::Integer))) {
        return true;
    }

    return false;
}

void SemanticValidator::validateComparison(ast::ComparisonExpr const* expr,
    catalog::TableSchema const& table) const
{

    types::DataType const leftType = inferType(expr->left(), table);
    types::DataType const rightType = inferType(expr->right(), table);

    if (!areComparable(leftType, rightType)) {
        throw SemanticError(SemanticErrorKind::TypeMismatch,
            "incompatible types in comparison");
    }
}

} // namespace hegadb
