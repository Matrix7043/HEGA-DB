#include <hegadb/client/SQLParser.h>
#include <hegadb/client/preparation/QueryPreparer.h>
#include <hegadb/client/protocol/ExecuteQueryEncoder.h>
#include <hegadb/crypto/CompositeEncryption.h>

#include "common.pb.h"
#include "query.pb.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>

namespace proto = hegadb::protocol;

namespace {

constexpr std::uint32_t PROTOCOL_MAJOR_VERSION = 1;
constexpr std::uint32_t PROTOCOL_MINOR_VERSION = 1;

// ---------------------------------------------------------
// Test 1: Protocol version
// ---------------------------------------------------------

void testProtocolVersion()
{
    proto::ProtocolVersion version;
    version.set_major(PROTOCOL_MAJOR_VERSION);
    version.set_minor(PROTOCOL_MINOR_VERSION);

    assert(version.major() == PROTOCOL_MAJOR_VERSION);
    assert(version.minor() == PROTOCOL_MINOR_VERSION);

    std::cout << "[PASS] Protocol version\n";
}

// ---------------------------------------------------------
// Test 2: Request ID
// ---------------------------------------------------------

void testRequestId()
{
    proto::RequestId request_id;
    request_id.set_value("protocol-test-001");

    assert(request_id.value() == "protocol-test-001");

    std::cout << "[PASS] Request ID\n";
}

// ---------------------------------------------------------
// Test 3: Protocol enums
// ---------------------------------------------------------

void testProtocolEnums()
{
    assert(proto::COMPARISON_OP_UNSPECIFIED == 0);
    assert(proto::COMPARISON_OP_EQ != proto::COMPARISON_OP_UNSPECIFIED);
    assert(proto::COMPARISON_OP_NEQ != proto::COMPARISON_OP_UNSPECIFIED);
    assert(proto::COMPARISON_OP_LT != proto::COMPARISON_OP_UNSPECIFIED);
    assert(proto::COMPARISON_OP_GT != proto::COMPARISON_OP_UNSPECIFIED);
    assert(proto::COMPARISON_OP_LTE != proto::COMPARISON_OP_UNSPECIFIED);
    assert(proto::COMPARISON_OP_GTE != proto::COMPARISON_OP_UNSPECIFIED);

    assert(proto::LOGICAL_OP_UNSPECIFIED == 0);
    assert(proto::LOGICAL_OP_AND != proto::LOGICAL_OP_UNSPECIFIED);
    assert(proto::LOGICAL_OP_OR != proto::LOGICAL_OP_UNSPECIFIED);

    assert(proto::STATUS_OK == 0);
    assert(proto::STATUS_ERROR != proto::STATUS_OK);
    assert(proto::STATUS_SYNTAX_ERROR != proto::STATUS_OK);

    assert(proto::MESSAGE_TYPE_UNSPECIFIED == 0);
    assert(proto::QUERY_REQUEST != proto::MESSAGE_TYPE_UNSPECIFIED);
    assert(proto::QUERY_RESPONSE != proto::MESSAGE_TYPE_UNSPECIFIED);
    assert(proto::ERROR_RESPONSE != proto::MESSAGE_TYPE_UNSPECIFIED);

    std::cout << "[PASS] Protocol enums\n";
}

// ---------------------------------------------------------
// Test 4: QueryResponse
// ---------------------------------------------------------

void testQueryResponse()
{
    proto::QueryResponse response;

    response.mutable_protocol_version()->set_major(
        PROTOCOL_MAJOR_VERSION);

    response.mutable_protocol_version()->set_minor(
        PROTOCOL_MINOR_VERSION);

    response.mutable_request_id()->set_value(
        "protocol-test-002");

    response.mutable_status()->set_code(proto::STATUS_OK);
    response.mutable_status()->set_message(
        "Query executed successfully");

    assert(response.protocol_version().major() == PROTOCOL_MAJOR_VERSION);
    assert(response.protocol_version().minor() == PROTOCOL_MINOR_VERSION);
    assert(response.request_id().value() == "protocol-test-002");
    assert(response.status().code() == proto::STATUS_OK);
    assert(response.status().message() == "Query executed successfully");

    std::cout << "[PASS] QueryResponse\n";
}

// ---------------------------------------------------------
// Test 5: ExecuteQuery serialization round trip
// Uses actual CompositeEncryption and encoder
// ---------------------------------------------------------

void testExecuteQuerySerialization()
{
    hegadb::client::SQLParser parser;

    hegadb::crypto::CompositeEncryption encryption;

    hegadb::client::preparation::QueryPreparer preparer(
        encryption);

    auto ast = parser.parseToAST(
        "SELECT NAME FROM STUDENTS WHERE CLASS = 10;");

    assert(ast != nullptr);

    auto prepared = preparer.prepare(std::move(ast));

    // Ensure a real encrypted parameter was prepared.
    assert(prepared.parameters().size() == 1);

    auto request = hegadb::client::protocol::ExecuteQueryEncoder::encode(
        prepared,
        "protocol-test-003");

    // Verify the encoded message before serialization.
    assert(request.protocol_version().major() == PROTOCOL_MAJOR_VERSION);
    assert(request.protocol_version().minor() == PROTOCOL_MINOR_VERSION);

    assert(request.request_id().value() == "protocol-test-003");

    assert(request.statement().has_select_statement());

    auto const& select = request.statement().select_statement();

    assert(select.from_table().table() == "STUDENTS");
    assert(select.projections_size() == 1);
    assert(select.projections(0).column_ref().column() == "NAME");

    assert(select.has_where());
    assert(select.where().has_comparison_expr());

    auto const& comparison = select.where().comparison_expr();

    assert(comparison.operator_() == proto::COMPARISON_OP_EQ);
    assert(comparison.left().column_ref().column() == "CLASS");
    assert(comparison.right().has_parameter_ref());
    assert(comparison.right().parameter_ref().parameter_index() == 0);

    // Verify encrypted parameter and keys.
    assert(request.parameters_size() == 1);
    assert(request.parameters(0).index() == 0);

    assert(
        request.parameters(0).ciphertext()
        == prepared.parameters().at(0).ciphertext().serialize());

    assert(request.has_keys());
    assert(request.keys().bfv_key() == prepared.bfvPublicKey());
    assert(request.keys().ckks_key() == prepared.ckksPublicKey());

    // Serialize.
    std::string payload;
    bool serialized = request.SerializeToString(&payload);

    assert(serialized);
    assert(!payload.empty());

    // Deserialize.
    proto::ExecuteQuery received;
    bool parsed = received.ParseFromString(payload);

    assert(parsed);

    // Verify metadata.
    assert(received.protocol_version().major() == PROTOCOL_MAJOR_VERSION);
    assert(received.protocol_version().minor() == PROTOCOL_MINOR_VERSION);
    assert(received.request_id().value() == "protocol-test-003");

    // Verify statement.
    assert(received.statement().has_select_statement());

    auto const& received_select = received.statement().select_statement();

    assert(received_select.from_table().table() == "STUDENTS");
    assert(received_select.projections_size() == 1);
    assert(received_select.projections(0).column_ref().column() == "NAME");

    // Verify WHERE.
    assert(received_select.has_where());

    auto const& received_comparison = received_select.where().comparison_expr();

    assert(received_comparison.operator_() == proto::COMPARISON_OP_EQ);
    assert(received_comparison.left().column_ref().column() == "CLASS");
    assert(received_comparison.right().parameter_ref().parameter_index() == 0);

    // Verify encrypted parameter survived unchanged.
    assert(received.parameters_size() == 1);
    assert(received.parameters(0).index() == 0);

    assert(
        received.parameters(0).ciphertext()
        == request.parameters(0).ciphertext());

    // Verify public keys survived unchanged.
    assert(received.has_keys());

    assert(received.keys().bfv_key() == request.keys().bfv_key());
    assert(received.keys().ckks_key() == request.keys().ckks_key());

    std::cout
        << "[PASS] ExecuteQuery serialization round trip"
        << " (" << payload.size() << " bytes)\n";
}

// ---------------------------------------------------------
// Test 6: Invalid payload
// ---------------------------------------------------------

void testInvalidPayload()
{
    std::string invalid_payload = "not-a-valid-protobuf-message";

    proto::ExecuteQuery request;

    bool parsed = request.ParseFromString(invalid_payload);

    assert(!parsed);

    std::cout << "[PASS] Invalid protobuf payload rejection\n";
}

} // namespace

// ---------------------------------------------------------
// Main
// ---------------------------------------------------------

int main()
{
    std::cout
        << "=========================================================\n"
        << "              HEGADB PROTOCOL TESTS                     \n"
        << "=========================================================\n\n";

    testProtocolVersion();
    testRequestId();
    testProtocolEnums();
    testQueryResponse();
    testExecuteQuerySerialization();
    testInvalidPayload();

    std::cout
        << "\n=========================================================\n"
        << "           ALL PROTOCOL TESTS PASSED                    \n"
        << "=========================================================\n";

    return 0;
}
