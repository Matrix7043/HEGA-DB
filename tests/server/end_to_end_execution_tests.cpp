#include <hegadb/server/execution/QueryExecutor.h>
#include <hegadb/server/api/ExecuteQueryReceiver.h>
#include <hegadb/catalog/db-client/InMemoryCatalog.h>
#include <hegadb/catalog/TableSchema.h>
#include <hegadb/crypto/OpenFHEContext.h>
#include <hegadb/server/storage/StorageManager.h>
#include <gtest/gtest.h>

using namespace hegadb::server;
using namespace hegadb::server::execution;
using namespace hegadb::catalog;
using namespace hegadb::crypto;
using namespace hegadb::types;
using namespace hegadb::protocol;

// Mock storage manager for testing
class MockStorageManager : public storage::StorageManager {
public:
    std::unique_ptr<storage::EncryptedTable> openTable(
        std::string const& table_name) override {
        // Return mock table for testing
        throw std::runtime_error("Mock table not implemented");
    }
};

class EndToEndExecutionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup catalog
        std::vector<ColumnSchema> columns = {
            {"id",     hegadb::types::DataType::Integer, false, 0},
            {"salary", hegadb::types::DataType::Integer, false, 1}
        };

        auto table = std::make_shared<TableSchema>("employees", columns);
        
        // Create an empty catalog snapshot
        std::unordered_map<std::string, TableSchema> tables;
        auto snapshot = std::make_shared<CatalogSnapshot const>(1, std::move(tables));
        
        catalog_ = std::make_shared<InMemoryCatalog>(snapshot);

        // Setup crypto context
        crypto_context_ = std::make_shared<OpenFHEContext>();
        crypto_context_->enableFeatures();

        // Setup storage
        storage_ = std::make_shared<MockStorageManager>();

        // Create executor
        executor_ = std::make_shared<QueryExecutor>(
            catalog_,
            storage_,
            crypto_context_
        );

        // Create receiver
        receiver_ = std::make_unique<ExecuteQueryReceiver>(
            catalog_,
            executor_
        );

        // Generate keypair for encrypting parameters
        auto bfv_ctx = crypto_context_->getBFVContext();
        auto keypair = bfv_ctx->KeyGen();
        public_key_ = keypair.publicKey;
    }

    ExecuteQuery createSelectQuery(std::string const& table_name) {
        ExecuteQuery query;

        // Protocol version
        query.mutable_protocol_version()->set_major(1);
        query.mutable_protocol_version()->set_minor(0);

        // Request ID
        query.mutable_request_id()->set_value("test-001");

        // SELECT id FROM employees WHERE id = $0
        auto* select = query.mutable_statement()->mutable_select_statement();
        select->mutable_from_table()->set_table(table_name);

        auto* proj = select->add_projections();
        proj->mutable_column_ref()->set_column("id");

        // WHERE id = $0
        auto* where = select->mutable_where();
        auto* comp = where->mutable_comparison_expr();
        comp->set_operator_(COMPARISON_OP_EQ);
        comp->mutable_left()->mutable_column_ref()->set_column("id");
        comp->mutable_right()->mutable_parameter_ref()->set_parameter_index(0);

        // Result mode
        query.set_result_mode(RESULT_MODE_ROWS_AND_MASK);

        return query;
    }

    std::shared_ptr<Catalog> catalog_;
    std::shared_ptr<OpenFHEContext> crypto_context_;
    std::shared_ptr<storage::StorageManager> storage_;
    std::shared_ptr<QueryExecutor> executor_;
    std::unique_ptr<ExecuteQueryReceiver> receiver_;
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> public_key_;
};

TEST_F(EndToEndExecutionTest, ValidateProtocolVersion) {
    auto query = createSelectQuery("employees");

    // This should fail at storage level (mock), but protocol should be valid
    auto response = receiver_->executeQuery(query);

    EXPECT_EQ(response.protocol_version().major(), 1);
    EXPECT_EQ(response.request_id().value(), "test-001");
}

TEST_F(EndToEndExecutionTest, RejectInvalidProtocolVersion) {
    auto query = createSelectQuery("employees");
    query.mutable_protocol_version()->set_major(99);

    auto response = receiver_->executeQuery(query);

    EXPECT_EQ(response.status().code(), STATUS_ERROR);
    EXPECT_FALSE(response.status().message().empty());
}

TEST_F(EndToEndExecutionTest, QueryWithoutWhereClause) {
    ExecuteQuery query;
    query.mutable_protocol_version()->set_major(1);
    query.mutable_protocol_version()->set_minor(0);
    query.mutable_request_id()->set_value("test-002");

    // SELECT * FROM employees (no WHERE)
    auto* select = query.mutable_statement()->mutable_select_statement();
    select->mutable_from_table()->set_table("employees");
    select->add_projections()->mutable_wildcard();
    query.set_result_mode(RESULT_MODE_ROWS_AND_MASK);

    // Should be valid query structure
    EXPECT_NO_THROW({
        auto response = receiver_->executeQuery(query);
    });
}

TEST_F(EndToEndExecutionTest, QueryWithMultipleProjections) {
    ExecuteQuery query;
    query.mutable_protocol_version()->set_major(1);
    query.mutable_protocol_version()->set_minor(0);
    query.mutable_request_id()->set_value("test-003");

    auto* select = query.mutable_statement()->mutable_select_statement();
    select->mutable_from_table()->set_table("employees");

    select->add_projections()->mutable_column_ref()->set_column("id");
    select->add_projections()->mutable_column_ref()->set_column("salary");

    query.set_result_mode(RESULT_MODE_MASKED_ROWS);

    EXPECT_NO_THROW({
        auto response = receiver_->executeQuery(query);
    });
}
