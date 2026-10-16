#include "hegadb/client/protocol/ExecuteQueryEncoder.h"

#include <hegadb/client/ast/ColumnRef.h>
#include <hegadb/client/ast/ComparisonExpr.h>
#include <hegadb/client/ast/CreateTableStmt.h>
#include <hegadb/client/ast/DropTableStmt.h>
#include <hegadb/client/ast/JoinClause.h>
#include <hegadb/client/ast/LogicalExpr.h>
#include <hegadb/client/ast/NotExpr.h>
#include <hegadb/client/ast/ParameterRef.h>
#include <hegadb/client/ast/SelectStmt.h>
#include <hegadb/client/ast/Wildcard.h>

#include <hegadb/operations/ComparisonOp.h>
#include <hegadb/operations/LogicalOp.h>
#include <hegadb/types/DataType.h>

#include <cassert>
#include <stdexcept>
#include <utility>

namespace proto = hegadb::protocol;

namespace hegadb::client::protocol {

namespace {

constexpr uint32_t PROTOCOL_MAJOR_VERSION = 1;
constexpr uint32_t PROTOCOL_MINOR_VERSION = 1;

// Creates a request with the protocol version and request id filled in.
proto::ExecuteQuery makeRequest(std::string request_id)
{
    proto::ExecuteQuery request;

    request.mutable_protocol_version()->set_major(
        PROTOCOL_MAJOR_VERSION);

    request.mutable_protocol_version()->set_minor(
        PROTOCOL_MINOR_VERSION);

    request.mutable_request_id()->set_value(
        std::move(request_id));

    return request;
}

proto::DataType mapDataType(types::DataType type)
{
    switch (type) {
    case types::DataType::Integer:
        return proto::DATA_TYPE_INTEGER;

    case types::DataType::Float:
        return proto::DATA_TYPE_FLOAT;

    case types::DataType::String:
        return proto::DATA_TYPE_STRING;

    case types::DataType::Boolean:
        return proto::DATA_TYPE_BOOLEAN;
    }

    throw std::invalid_argument("Unknown DataType");
}

proto::ComparisonOp mapComparisonOp(
    operations::ComparisonOp op)
{
    switch (op) {
    case operations::ComparisonOp::EQ:
        return proto::COMPARISON_OP_EQ;

    case operations::ComparisonOp::NEQ:
        return proto::COMPARISON_OP_NEQ;

    case operations::ComparisonOp::LT:
        return proto::COMPARISON_OP_LT;

    case operations::ComparisonOp::GT:
        return proto::COMPARISON_OP_GT;

    case operations::ComparisonOp::LTE:
        return proto::COMPARISON_OP_LTE;

    case operations::ComparisonOp::GTE:
        return proto::COMPARISON_OP_GTE;
    }

    assert(false && "Unknown ComparisonOp");
    return proto::COMPARISON_OP_UNSPECIFIED;
}

proto::LogicalOp mapLogicalOp(
    operations::LogicalOp op)
{
    switch (op) {
    case operations::LogicalOp::AND:
        return proto::LOGICAL_OP_AND;

    case operations::LogicalOp::OR:
        return proto::LOGICAL_OP_OR;
    }

    assert(false && "Unknown LogicalOp");
    return proto::LOGICAL_OP_UNSPECIFIED;
}

void convertColumn(
    ast::ColumnRef const& column,
    proto::ColumnRef* proto_column)
{
    if (column.table()) {
        proto_column->set_table(*column.table());
    }

    proto_column->set_column(column.name());
}

void convertExpression(
    ast::Expr const* node,
    proto::Expression* proto_expr)
{
    assert(proto_expr != nullptr);

    if (node == nullptr) {
        return;
    }

    if (auto const* column = dynamic_cast<ast::ColumnRef const*>(node)) {

        convertColumn(*column, proto_expr->mutable_column_ref());

    } else if (auto const* parameter = dynamic_cast<ast::ParameterRef const*>(node)) {

        proto_expr->mutable_parameter_ref()->set_parameter_index(
            parameter->parameterIndex());

    } else if (
        dynamic_cast<ast::Wildcard const*>(node)) {

        proto_expr->mutable_wildcard();

    } else if (auto const* comparison = dynamic_cast<ast::ComparisonExpr const*>(node)) {

        auto* proto_comparison = proto_expr->mutable_comparison_expr();

        convertExpression(
            comparison->left(),
            proto_comparison->mutable_left());

        proto_comparison->set_operator_(
            mapComparisonOp(comparison->op()));

        convertExpression(
            comparison->right(),
            proto_comparison->mutable_right());

    } else if (auto const* logical = dynamic_cast<ast::LogicalExpr const*>(node)) {

        auto* proto_logical = proto_expr->mutable_logical_expr();

        convertExpression(
            logical->left(),
            proto_logical->mutable_left());

        proto_logical->set_operator_(
            mapLogicalOp(logical->op()));

        convertExpression(
            logical->right(),
            proto_logical->mutable_right());

    } else if (auto const* not_expr = dynamic_cast<ast::NotExpr const*>(node)) {

        auto* proto_not = proto_expr->mutable_not_expr();

        convertExpression(
            not_expr->child(),
            proto_not->mutable_child());
    }
}

} // namespace

proto::ExecuteQuery ExecuteQueryEncoder::encode(
    preparation::PreparedQuery const& prepared,
    std::string request_id)
{
    /*
     * -----------------------------------------------------
     * Protocol version and request ID
     * -----------------------------------------------------
     */

    proto::ExecuteQuery request = makeRequest(std::move(request_id));

    /*
     * -----------------------------------------------------
     * Prepared query
     * -----------------------------------------------------
     */

    auto const* select_ast = dynamic_cast<ast::SelectStmt const*>(
        &prepared.query());

    assert(
        select_ast != nullptr && "Prepared query must be a SelectStmt");

    auto* select = request.mutable_statement()
                       ->mutable_select_statement();

    /*
     * -----------------------------------------------------
     * FROM
     * -----------------------------------------------------
     */

    if (select_ast->from()) {
        select->mutable_from_table()->set_table(
            select_ast->from()->name());
    }

    /*
     * -----------------------------------------------------
     * INNER JOIN
     * -----------------------------------------------------
     */

    if (select_ast->join()) {
        auto const* join = select_ast->join();
        auto* proto_join = select->mutable_join();

        proto_join->mutable_right_table()->set_table(
            join->table()->name());

        convertColumn(*join->left(), proto_join->mutable_left_column());
        convertColumn(*join->right(), proto_join->mutable_right_column());
    }

    /*
     * -----------------------------------------------------
     * Projection list
     * -----------------------------------------------------
     */

    for (auto const& projection :
        select_ast->projections()) {

        convertExpression(
            projection.get(),
            select->add_projections());
    }

    /*
     * -----------------------------------------------------
     * WHERE
     * -----------------------------------------------------
     */

    if (select_ast->where()) {

        convertExpression(
            select_ast->where(),
            select->mutable_where());
    }

    /*
     * -----------------------------------------------------
     * Encrypted parameters
     * -----------------------------------------------------
     *
     * The current crypto layer does not expose serialized
     * ciphertext bytes. Therefore the v0.7 mock transport
     * representation is retained for now.
     */

    for (auto const& parameter :
        prepared.parameters()) {

        auto* proto_parameter = request.add_parameters();

        proto_parameter->set_index(
            parameter.index());

        proto_parameter->set_ciphertext(
            parameter.ciphertext().serialize());
    }

    /*
     * -----------------------------------------------------
     * Public Key
     * -----------------------------------------------------
     */

    if (!prepared.parameters().empty()) {
        request.mutable_keys()->set_bfv_key(prepared.bfvPublicKey());
        request.mutable_keys()->set_ckks_key(prepared.ckksPublicKey());
    }

    return request;
}

proto::ExecuteQuery ExecuteQueryEncoder::encode(
    ast::CreateTableStmt const& stmt,
    std::string request_id)
{
    proto::ExecuteQuery request = makeRequest(std::move(request_id));

    auto* create = request.mutable_statement()
                       ->mutable_create_table_statement();

    create->set_table(stmt.tableName());

    // Column order is preserved: it defines the column ordinals.
    for (auto const& column : stmt.columns()) {
        auto* proto_column = create->add_columns();

        proto_column->set_name(column.name);
        proto_column->set_data_type(mapDataType(column.type));
        proto_column->set_nullable(column.nullable);
    }

    return request;
}

proto::ExecuteQuery ExecuteQueryEncoder::encode(
    ast::DropTableStmt const& stmt,
    std::string request_id)
{
    proto::ExecuteQuery request = makeRequest(std::move(request_id));

    request.mutable_statement()
        ->mutable_drop_table_statement()
        ->set_table(stmt.tableName());

    return request;
}

proto::ExecuteQuery ExecuteQueryEncoder::encodeDdl(
    ast::Statement const& stmt,
    std::string request_id)
{
    switch (stmt.kind()) {

    case ast::ASTNodeKind::CreateTableStmt:
        return encode(
            static_cast<ast::CreateTableStmt const&>(stmt),
            std::move(request_id));

    case ast::ASTNodeKind::DropTableStmt:
        return encode(
            static_cast<ast::DropTableStmt const&>(stmt),
            std::move(request_id));

    default:
        throw std::invalid_argument(
            "ExecuteQueryEncoder::encodeDdl expects CREATE TABLE or DROP TABLE");
    }
}

} // namespace hegadb::client::protocol
