#include "hegadb/crypto/CompositeEncryption.h"
#include "hegadb/crypto/EncryptionWrapper.h"

#include <hegadb/client/SQLParser.h>
#include <hegadb/client/ast/ASTParameterizer.h>
#include <hegadb/client/ast/CreateTableStmt.h>
#include <hegadb/client/ast/InsertStmt.h>
#include <hegadb/client/ast/Literal.h>
#include <hegadb/client/ast/NullLiteral.h>
#include <hegadb/client/ast/ParameterRef.h>
#include <hegadb/client/preparation/LiteralExtractor.h>
#include <hegadb/client/preparation/PreparedQuery.h>
#include <hegadb/client/preparation/QueryPreparer.h>
#include <hegadb/client/protocol/ExecuteQueryEncoder.h>

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>
#include <string>
#include <variant>

namespace {

using hegadb::client::SQLParser;
using hegadb::client::ast::ASTNodeKind;
using hegadb::client::ast::ASTParameterizer;
using hegadb::client::ast::InsertStmt;
using hegadb::client::ast::Literal;
using hegadb::client::ast::ParameterRef;
using hegadb::client::preparation::LiteralExtractor;
using hegadb::client::preparation::PreparedQuery;
using hegadb::client::protocol::ExecuteQueryEncoder;
namespace proto = hegadb::protocol;

std::unique_ptr<hegadb::client::ast::Statement> parse(std::string const& sql)
{
    SQLParser parser;
    auto ast = parser.parseToAST(sql);
    EXPECT_NE(ast, nullptr) << sql;
    return ast;
}

// ============================================================================
// Literal extraction
// ============================================================================

TEST(InsertExtractionTest, ExtractsValuesRowByRowLeftToRight)
{
    auto ast = parse("INSERT INTO t VALUES (1, 'a', TRUE), (2, 'b', FALSE);");

    auto literals = LiteralExtractor().extract(*ast);

    ASSERT_EQ(literals.size(), 6u);

    for (std::size_t i = 0; i < literals.size(); ++i) {
        EXPECT_EQ(literals[i].parameter_index, i);
    }

    EXPECT_EQ(std::get<long long>(literals[0].value), 1);
    EXPECT_EQ(std::get<std::string>(literals[1].value), "a");
    EXPECT_EQ(std::get<bool>(literals[2].value), true);
    EXPECT_EQ(std::get<long long>(literals[3].value), 2);
    EXPECT_EQ(std::get<std::string>(literals[4].value), "b");
    EXPECT_EQ(std::get<bool>(literals[5].value), false);
}

TEST(InsertExtractionTest, NullIsNotExtracted)
{
    auto ast = parse("INSERT INTO t (a, b, c) VALUES (1, NULL, 'x'), (NULL, NULL, 'y');");

    auto literals = LiteralExtractor().extract(*ast);

    // Three values are not NULL, and they are numbered without gaps.
    ASSERT_EQ(literals.size(), 3u);
    EXPECT_EQ(literals[0].parameter_index, 0u);
    EXPECT_EQ(literals[1].parameter_index, 1u);
    EXPECT_EQ(literals[2].parameter_index, 2u);
    EXPECT_EQ(std::get<long long>(literals[0].value), 1);
    EXPECT_EQ(std::get<std::string>(literals[1].value), "x");
    EXPECT_EQ(std::get<std::string>(literals[2].value), "y");
}

TEST(InsertExtractionTest, AllNullExtractsNothing)
{
    auto ast = parse("INSERT INTO t VALUES (NULL, NULL);");

    EXPECT_TRUE(LiteralExtractor().extract(*ast).empty());
}

// ============================================================================
// Parameterization
// ============================================================================

TEST(InsertParameterizerTest, ReplacesLiteralsAndKeepsNull)
{
    auto ast = parse(
        "INSERT INTO users (id, name, active) VALUES (1, 'alice', TRUE), (2, NULL, FALSE);");

    auto result = ASTParameterizer().parameterize(*ast);

    ASSERT_EQ(result->kind(), ASTNodeKind::InsertStmt);
    auto const& insert = static_cast<InsertStmt const&>(*result);

    EXPECT_EQ(insert.tableName(), "users");
    ASSERT_EQ(insert.columns().size(), 3u);
    EXPECT_EQ(insert.columns()[0], "id");
    EXPECT_EQ(insert.columns()[1], "name");
    EXPECT_EQ(insert.columns()[2], "active");

    ASSERT_EQ(insert.rows().size(), 2u);

    // Row 0: parameters 0, 1, 2
    for (std::size_t i = 0; i < 3; ++i) {
        auto const& cell = insert.rows()[0][i];
        ASSERT_EQ(cell->kind(), ASTNodeKind::ParameterRef);
        EXPECT_EQ(static_cast<ParameterRef const&>(*cell).parameterIndex(), i);
    }

    // Row 1: parameter 3, then NULL, then parameter 4
    ASSERT_EQ(insert.rows()[1][0]->kind(), ASTNodeKind::ParameterRef);
    EXPECT_EQ(static_cast<ParameterRef const&>(*insert.rows()[1][0]).parameterIndex(), 3u);

    EXPECT_EQ(insert.rows()[1][1]->kind(), ASTNodeKind::NullLiteral);

    ASSERT_EQ(insert.rows()[1][2]->kind(), ASTNodeKind::ParameterRef);
    EXPECT_EQ(static_cast<ParameterRef const&>(*insert.rows()[1][2]).parameterIndex(), 4u);
}

TEST(InsertParameterizerTest, NoPlaintextLiteralRemains)
{
    auto ast = parse("INSERT INTO t VALUES (1, 'secret', TRUE);");

    auto result = ASTParameterizer().parameterize(*ast);
    auto const& insert = static_cast<InsertStmt const&>(*result);

    for (auto const& cell : insert.rows()[0]) {
        EXPECT_NE(cell->kind(), ASTNodeKind::Literal);
    }
}

TEST(InsertParameterizerTest, DoesNotChangeTheOriginal)
{
    auto ast = parse("INSERT INTO t VALUES (1, NULL);");

    auto result = ASTParameterizer().parameterize(*ast);
    (void)result;

    auto const& insert = static_cast<InsertStmt const&>(*ast);
    EXPECT_EQ(insert.rows()[0][0]->kind(), ASTNodeKind::Literal);
    EXPECT_EQ(insert.rows()[0][1]->kind(), ASTNodeKind::NullLiteral);
}

TEST(InsertParameterizerTest, IndicesMatchTheExtractor)
{
    auto ast = parse("INSERT INTO t VALUES (1, NULL, 'a'), ('b', 2, NULL);");

    auto literals = LiteralExtractor().extract(*ast);
    auto result = ASTParameterizer().parameterize(*ast);
    auto const& insert = static_cast<InsertStmt const&>(*result);

    // The i-th parameter in the statement is the i-th extracted literal.
    std::size_t expected = 0;

    for (auto const& row : insert.rows()) {
        for (auto const& cell : row) {
            if (cell->kind() == ASTNodeKind::ParameterRef) {
                EXPECT_EQ(static_cast<ParameterRef const&>(*cell).parameterIndex(), expected);
                EXPECT_EQ(literals.at(expected).parameter_index, expected);
                ++expected;
            }
        }
    }

    EXPECT_EQ(expected, literals.size());
}

TEST(InsertParameterizerTest, StillRejectsOtherStatements)
{
    auto ast = parse("CREATE TABLE t (a INTEGER);");

    EXPECT_THROW(ASTParameterizer().parameterize(*ast), std::runtime_error);
}

// ============================================================================
// Query preparation
// ============================================================================

// Wraps a real CompositeEncryption and records which encryption call was
// used for which kind of value.
class RecordingEncryption final : public hegadb::crypto::EncryptionWrapper {
public:
    std::unique_ptr<hegadb::crypto::Ciphertext> encrypt(
        hegadb::crypto::Plaintext const& value) override
    {
        encrypt_calls.push_back(value.type());
        return inner_.encrypt(value);
    }

    std::unique_ptr<hegadb::crypto::Ciphertext> encryptInsert(
        hegadb::crypto::Plaintext const& value) override
    {
        insert_calls.push_back(value.type());
        return inner_.encryptInsert(value);
    }

    hegadb::crypto::Plaintext decrypt(hegadb::crypto::Ciphertext const& value) override
    {
        return inner_.decrypt(value);
    }

    hegadb::crypto::KeyManager const& getKeyManager() const override
    {
        return inner_.getKeyManager();
    }

    std::vector<hegadb::types::DataType> encrypt_calls;
    std::vector<hegadb::types::DataType> insert_calls;

private:
    hegadb::crypto::CompositeEncryption inner_;
};

TEST(InsertPreparationTest, UsesEncryptInsertForEveryValue)
{
    using hegadb::types::DataType;

    RecordingEncryption encryption;
    hegadb::client::preparation::QueryPreparer preparer(encryption);

    auto prepared = preparer.prepare(
        parse("INSERT INTO t VALUES (1, 'x', TRUE, NULL), (2, 'y', FALSE, NULL);"));

    // Every non-NULL value, in order, through encryptInsert and nothing else.
    EXPECT_TRUE(encryption.encrypt_calls.empty());

    std::vector<DataType> const expected = {
        DataType::Integer, DataType::String, DataType::Boolean,
        DataType::Integer, DataType::String, DataType::Boolean
    };
    EXPECT_EQ(encryption.insert_calls, expected);

    EXPECT_EQ(prepared.parameters().size(), 6u);
}

TEST(InsertPreparationTest, NullIsNeverEncrypted)
{
    RecordingEncryption encryption;
    hegadb::client::preparation::QueryPreparer preparer(encryption);

    (void)preparer.prepare(parse("INSERT INTO t VALUES (NULL, NULL);"));

    EXPECT_TRUE(encryption.insert_calls.empty());
    EXPECT_TRUE(encryption.encrypt_calls.empty());
}

TEST(InsertPreparationTest, SelectStillUsesEncrypt)
{
    using hegadb::types::DataType;

    RecordingEncryption encryption;
    hegadb::client::preparation::QueryPreparer preparer(encryption);

    (void)preparer.prepare(parse("SELECT * FROM t WHERE a = 5 AND b = 'x';"));

    // Comparison literals keep using the normal encryption.
    EXPECT_TRUE(encryption.insert_calls.empty());
    ASSERT_EQ(encryption.encrypt_calls.size(), 2u);
    EXPECT_EQ(encryption.encrypt_calls[0], DataType::Integer);
    EXPECT_EQ(encryption.encrypt_calls[1], DataType::String);
}

TEST(InsertPreparationTest, EncryptsEveryNonNullValue)
{
    hegadb::crypto::CompositeEncryption encryption;
    hegadb::client::preparation::QueryPreparer preparer(encryption);

    auto prepared = preparer.prepare(
        parse("INSERT INTO t (a, b, c) VALUES (1, NULL, 'x'), (2, 'y', NULL);"));

    ASSERT_EQ(prepared.query().kind(), ASTNodeKind::InsertStmt);

    // 1, 'x', 2 and 'y' are encrypted. The two NULLs are not.
    ASSERT_EQ(prepared.parameters().size(), 4u);
    for (std::size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(prepared.parameters()[i].index(), i);
    }

    EXPECT_FALSE(prepared.bfvPublicKey().empty());
    EXPECT_FALSE(prepared.ckksPublicKey().empty());
}

TEST(InsertPreparationTest, AllNullNeedsNoKeys)
{
    hegadb::crypto::CompositeEncryption encryption;
    hegadb::client::preparation::QueryPreparer preparer(encryption);

    auto prepared = preparer.prepare(parse("INSERT INTO t VALUES (NULL, NULL);"));

    EXPECT_TRUE(prepared.parameters().empty());
    EXPECT_TRUE(prepared.bfvPublicKey().empty());
    EXPECT_TRUE(prepared.ckksPublicKey().empty());
}

// ============================================================================
// Encoder
// ============================================================================

class InsertEncoderTest : public ::testing::Test {
protected:
    proto::ExecuteQuery encodeSql(std::string const& sql, std::string request_id = "req-insert")
    {
        auto prepared = preparer_.prepare(parse(sql));
        return ExecuteQueryEncoder::encode(prepared, std::move(request_id));
    }

    hegadb::crypto::CompositeEncryption encryption_;
    hegadb::client::preparation::QueryPreparer preparer_ { encryption_ };
};

TEST_F(InsertEncoderTest, EncodesAllFields)
{
    auto request = encodeSql(
        "INSERT INTO users (id, name, active) VALUES (1, 'alice', TRUE), (2, NULL, FALSE);");

    EXPECT_EQ(request.protocol_version().major(), 1u);
    EXPECT_EQ(request.protocol_version().minor(), 1u);
    EXPECT_EQ(request.request_id().value(), "req-insert");

    ASSERT_TRUE(request.statement().has_insert_statement());
    auto const& insert = request.statement().insert_statement();

    EXPECT_EQ(insert.table(), "users");

    ASSERT_EQ(insert.columns_size(), 3);
    EXPECT_EQ(insert.columns(0), "id");
    EXPECT_EQ(insert.columns(1), "name");
    EXPECT_EQ(insert.columns(2), "active");

    ASSERT_EQ(insert.rows_size(), 2);
    ASSERT_EQ(insert.rows(0).values_size(), 3);
    ASSERT_EQ(insert.rows(1).values_size(), 3);

    // Row 0: parameters 0, 1, 2
    for (int i = 0; i < 3; ++i) {
        auto const& value = insert.rows(0).values(i);
        ASSERT_TRUE(value.has_parameter_ref());
        EXPECT_EQ(value.parameter_ref().parameter_index(), static_cast<uint32_t>(i));
    }

    // Row 1: parameter 3, NULL, parameter 4
    ASSERT_TRUE(insert.rows(1).values(0).has_parameter_ref());
    EXPECT_EQ(insert.rows(1).values(0).parameter_ref().parameter_index(), 3u);

    EXPECT_TRUE(insert.rows(1).values(1).has_null_value());
    EXPECT_FALSE(insert.rows(1).values(1).has_parameter_ref());

    ASSERT_TRUE(insert.rows(1).values(2).has_parameter_ref());
    EXPECT_EQ(insert.rows(1).values(2).parameter_ref().parameter_index(), 4u);

    // Five encrypted parameters and the public keys travel with it.
    ASSERT_EQ(request.parameters_size(), 5);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(request.parameters(i).index(), static_cast<uint32_t>(i));
    }
    EXPECT_TRUE(request.has_keys());
    EXPECT_FALSE(request.keys().bfv_key().empty());
    EXPECT_FALSE(request.keys().ckks_key().empty());
}

TEST_F(InsertEncoderTest, WithoutColumnListSendsNoColumns)
{
    auto request = encodeSql("INSERT INTO users VALUES (1, 'alice');");

    ASSERT_TRUE(request.statement().has_insert_statement());
    EXPECT_EQ(request.statement().insert_statement().columns_size(), 0);
    EXPECT_EQ(request.statement().insert_statement().rows_size(), 1);
}

TEST_F(InsertEncoderTest, StatementCarriesNoPlaintext)
{
    auto request = encodeSql(
        "INSERT INTO users (id, name) VALUES (123456789, 'very-secret-name');");

    // The statement refers to the values by parameter index only.
    auto const wire = request.statement().SerializeAsString();

    EXPECT_EQ(wire.find("very-secret-name"), std::string::npos);

    auto const& insert = request.statement().insert_statement();
    for (auto const& value : insert.rows(0).values()) {
        EXPECT_TRUE(value.has_parameter_ref());
    }
}

TEST_F(InsertEncoderTest, AllNullSendsNoParametersAndNoKeys)
{
    auto request = encodeSql("INSERT INTO t (a, b) VALUES (NULL, NULL);");

    ASSERT_TRUE(request.statement().has_insert_statement());
    auto const& insert = request.statement().insert_statement();

    ASSERT_EQ(insert.rows_size(), 1);
    EXPECT_TRUE(insert.rows(0).values(0).has_null_value());
    EXPECT_TRUE(insert.rows(0).values(1).has_null_value());

    EXPECT_EQ(request.parameters_size(), 0);
    EXPECT_FALSE(request.has_keys());
}

TEST_F(InsertEncoderTest, SurvivesTheWire)
{
    auto request = encodeSql("INSERT INTO t (a, b) VALUES (1, NULL), (2, 'x');");

    proto::ExecuteQuery decoded;
    ASSERT_TRUE(decoded.ParseFromString(request.SerializeAsString()));

    ASSERT_TRUE(decoded.statement().has_insert_statement());
    EXPECT_EQ(decoded.statement().insert_statement().rows_size(), 2);
    EXPECT_TRUE(decoded.statement().insert_statement().rows(0).values(1).has_null_value());
    EXPECT_EQ(decoded.parameters_size(), 3);
}

TEST_F(InsertEncoderTest, RefusesAnInsertThatWasNotParameterized)
{
    // A statement that still holds plaintext literals must not be encoded.
    PreparedQuery prepared(parse("INSERT INTO t VALUES (1, 'secret');"), {});

    EXPECT_THROW(ExecuteQueryEncoder::encode(prepared, "r"), std::invalid_argument);
}

TEST_F(InsertEncoderTest, RefusesOtherStatements)
{
    PreparedQuery prepared(parse("CREATE TABLE t (a INTEGER);"), {});

    EXPECT_THROW(ExecuteQueryEncoder::encode(prepared, "r"), std::invalid_argument);
}

TEST_F(InsertEncoderTest, EncodeDdlRejectsInsert)
{
    auto insert = parse("INSERT INTO t VALUES (1);");

    EXPECT_THROW(ExecuteQueryEncoder::encodeDdl(*insert, "r"), std::invalid_argument);
}

TEST_F(InsertEncoderTest, SelectStillEncodes)
{
    auto prepared = preparer_.prepare(parse("SELECT * FROM t WHERE a = 5;"));
    auto request = ExecuteQueryEncoder::encode(prepared, "r");

    EXPECT_TRUE(request.statement().has_select_statement());
    EXPECT_FALSE(request.statement().has_insert_statement());
    EXPECT_EQ(request.parameters_size(), 1);
}

} // namespace
