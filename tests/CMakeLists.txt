# ============================================================
# Client Tests
# ============================================================

add_executable(parser_tests
    client/parser_tests.cpp
)

target_link_libraries(parser_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME parser_tests
    COMMAND parser_tests
)

add_executable(parser_insert_tests
    client/parser_insert_tests.cpp
)

target_link_libraries(parser_insert_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME parser_insert_tests
    COMMAND parser_insert_tests
)


add_executable(ast_tests
    client/ast_tests.cpp
)

target_link_libraries(ast_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME ast_tests
    COMMAND ast_tests
)


add_executable(ast_builder_tests
    client/ast_builder_tests.cpp
)

target_include_directories(ast_builder_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/client/generated
        /usr/include/antlr4-runtime
)

target_link_libraries(ast_builder_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME ast_builder_tests
    COMMAND ast_builder_tests
)

add_executable(ast_builder_ddl_tests
    client/ast_builder_ddl_tests.cpp
)

target_link_libraries(ast_builder_ddl_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME ast_builder_ddl_tests
    COMMAND ast_builder_ddl_tests
)

add_executable(ast_builder_insert_tests
    client/ast_builder_insert_tests.cpp
)

target_link_libraries(ast_builder_insert_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME ast_builder_insert_tests
    COMMAND ast_builder_insert_tests
)

add_executable(ast_parameterizer_tests
    client/ast_parameterizer_tests.cpp
)

target_link_libraries(ast_parameterizer_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME ast_parameterizer_tests
    COMMAND ast_parameterizer_tests
)


add_executable(semantic_validator_tests
    client/semantic_validator_tests.cpp
)

target_link_libraries(semantic_validator_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME semantic_validator_tests
    COMMAND semantic_validator_tests
)

add_executable(semantic_validator_ddl_tests
    client/semantic_validator_ddl_tests.cpp
)

target_link_libraries(semantic_validator_ddl_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME semantic_validator_ddl_tests
    COMMAND semantic_validator_ddl_tests
)

add_executable(semantic_validator_insert_tests
    client/semantic_validator_insert_tests.cpp
)

target_link_libraries(semantic_validator_insert_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME semantic_validator_insert_tests
    COMMAND semantic_validator_insert_tests
)


add_executable(literal_extractor_tests
    client/literal_extractor_tests.cpp
)

target_link_libraries(literal_extractor_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME literal_extractor_tests
    COMMAND literal_extractor_tests
)


add_executable(query_preparer_tests
    client/query_preparer_tests.cpp
)

target_link_libraries(query_preparer_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME query_preparer_tests
    COMMAND query_preparer_tests
)

add_executable(execute_query_encoder_tests
    client/execute_query_encoder_tests.cpp
)

target_link_libraries(execute_query_encoder_tests
    PRIVATE
      hegadb_client_core
)

add_test(
    NAME execute_query_encoder_tests
    COMMAND execute_query_encoder_tests
)

# ============================================================
# Catalog Tests
# ============================================================

add_executable(catalog_manager_tests
    catalog/catalog_manager_tests.cpp
)

target_link_libraries(catalog_manager_tests
    PRIVATE
        hegadb_catalog_service
)

add_test(
    NAME catalog_manager_tests
    COMMAND catalog_manager_tests
)

add_executable(catalog_adapter_tests
    catalog/catalog_adapter_tests.cpp
)

target_link_libraries(catalog_adapter_tests
    PRIVATE
        hegadb_catalog_client
)

add_test(
    NAME catalog_adapter_tests
    COMMAND catalog_adapter_tests
)

add_executable(catalog_cache_tests
    catalog/catalog_cache_tests.cpp
)

target_link_libraries(catalog_cache_tests
    PRIVATE
        hegadb_catalog_client
)

add_test(
    NAME catalog_cache_tests
    COMMAND catalog_cache_tests
)

add_executable(catalog_service_tests
    catalog/catalog_service_tests.cpp
)

target_link_libraries(catalog_service_tests
    PRIVATE
        hegadb_catalog_service
)

add_test(
    NAME catalog_service_tests
    COMMAND catalog_service_tests
)


add_executable(catalog_service_integration_tests
    catalog/catalog_service_integration_tests.cpp
)

target_link_libraries(catalog_service_integration_tests
    PRIVATE
        hegadb_catalog_service
        hegadb_catalog_client_transport
        hegadb_catalog_mutation_client_transport
)

add_test(
    NAME catalog_service_integration_tests
    COMMAND catalog_service_integration_tests
)

# ============================================================
# Protocol Tests
# ============================================================

add_executable(hegadb_protocol_test
    protocol/protocol_tests.cpp
)

target_link_libraries(hegadb_protocol_test
    PRIVATE
        hegadb_protocol
        hegadb_client_core
        hegadb_crypto
)

add_test(
    NAME hegadb_protocol_test
    COMMAND hegadb_protocol_test
)


# ============================================================
# Crypto Tests
# ============================================================

add_executable(crypto_basic_tests
    crypto/crypto_basic_tests.cpp
)

target_link_libraries(crypto_basic_tests
    PRIVATE
        hegadb_crypto
)

add_test(
    NAME crypto_basic_tests
    COMMAND crypto_basic_tests
)

add_executable(crypto_string_tests
    crypto/crypto_string_tests.cpp
)

target_link_libraries(crypto_string_tests
    PRIVATE
        hegadb_crypto
)

add_test(
    NAME crypto_string_tests
    COMMAND crypto_string_tests
)

add_executable(crypto_context_tests
    crypto/crypto_context_tests.cpp
)

target_link_libraries(crypto_context_tests
    PRIVATE
        hegadb_crypto
)

add_test(
    NAME crypto_context_tests
    COMMAND crypto_context_tests
)


add_executable(crypto_serialization_tests
    crypto/crypto_serialization_tests.cpp
)

target_link_libraries(crypto_serialization_tests
    PRIVATE
        hegadb_crypto
        hegadb_protocol
)

add_test(
    NAME crypto_serialization_tests
    COMMAND crypto_serialization_tests
)

add_executable(key_serialization_tests
    crypto/key_serialization_tests.cpp
)

target_link_libraries(key_serialization_tests
    PRIVATE
        hegadb_crypto
        GTest::gtest_main
)

add_test(
    NAME key_serialization_tests
    COMMAND key_serialization_tests
)

add_executable(key_persistence_tests
    crypto/key_persistence_tests.cpp
)

target_link_libraries(key_persistence_tests
    PRIVATE
        hegadb_crypto
        GTest::gtest_main
)

add_test(
    NAME key_persistence_tests
    COMMAND key_persistence_tests
)

add_executable(key_persistence_integration_test
    crypto/key_persistence_integration_test.cpp
)

target_link_libraries(key_persistence_integration_test
    PRIVATE
        hegadb_crypto
)

add_test(
    NAME key_persistence_integration_test
    COMMAND key_persistence_integration_test
)

add_executable(insert_slot_encryption_test
    crypto/insert_slot_encryption_test.cpp
)

target_link_libraries(insert_slot_encryption_test
    PRIVATE
        hegadb_crypto
        OPENFHEpke
        OPENFHEcore
)

add_test(
    NAME insert_slot_encryption_test
    COMMAND insert_slot_encryption_test
)


# ============================================================
# gRPC Tests
# ============================================================

find_package(GTest CONFIG REQUIRED)

add_executable(hegadb_grpc_tests
  grpc/grpc_tests.cpp
)

target_link_libraries(hegadb_grpc_tests
  hegadb_server_transport
  hegadb_client_transport
  GTest::gtest_main
)

add_executable(hegadb_catalog_transport_tests
  grpc/catalog_transport_tests.cpp
)

target_link_libraries(hegadb_catalog_transport_tests
  PRIVATE
    hegadb_catalog_server_transport
    hegadb_catalog_client_transport
)

add_test(
    NAME hegadb_catalog_transport_tests
    COMMAND hegadb_catalog_transport_tests
)

include(GoogleTest)

gtest_discover_tests(hegadb_grpc_tests)


# ============================================================
# Server Receiver Tests
# ============================================================

add_executable(server_receiver_tests
    server/server_receiver_tests.cpp
)

target_link_libraries(server_receiver_tests
    PRIVATE
        hegadb_server
        hegadb_server_transport
        hegadb_client_transport
        GTest::gtest_main
)

gtest_discover_tests(server_receiver_tests)


# ============================================================
# DDL protocol tests (encoder + client -> gRPC -> server)
# ============================================================

add_executable(ddl_protocol_tests
    client/ddl_protocol_tests.cpp
)

target_link_libraries(ddl_protocol_tests
    PRIVATE
        hegadb_client_core
        hegadb_server
        hegadb_server_transport
        hegadb_client_transport
        GTest::gtest_main
)

gtest_discover_tests(ddl_protocol_tests)


# ============================================================
# INSERT protocol tests (extraction, parameterization, preparation, encoder)
# ============================================================

add_executable(insert_protocol_tests
    client/insert_protocol_tests.cpp
)

target_link_libraries(insert_protocol_tests
    PRIVATE
        hegadb_client_core
        GTest::gtest_main
)

gtest_discover_tests(insert_protocol_tests)


# ============================================================
# INSERT server tests (receiver + client -> gRPC -> server)
# ============================================================

add_executable(insert_server_tests
    client/insert_server_tests.cpp
)

target_link_libraries(insert_server_tests
    PRIVATE
        hegadb_client_core
        hegadb_server
        hegadb_server_transport
        hegadb_client_transport
        GTest::gtest_main
)

gtest_discover_tests(insert_server_tests)


# ============================================================
# JOIN tests (AST, semantic validation, protocol + gRPC)
# ============================================================

add_executable(ast_builder_join_tests
    client/ast_builder_join_tests.cpp
)

target_link_libraries(ast_builder_join_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME ast_builder_join_tests
    COMMAND ast_builder_join_tests
)

add_executable(semantic_validator_join_tests
    client/semantic_validator_join_tests.cpp
)

target_link_libraries(semantic_validator_join_tests
    PRIVATE
        hegadb_client_core
)

add_test(
    NAME semantic_validator_join_tests
    COMMAND semantic_validator_join_tests
)

add_executable(join_protocol_tests
    client/join_protocol_tests.cpp
)

target_link_libraries(join_protocol_tests
    PRIVATE
        hegadb_client_core
        hegadb_server
        hegadb_server_transport
        hegadb_client_transport
        GTest::gtest_main
)

gtest_discover_tests(join_protocol_tests)


# ============================================================
# Server Storage Tests
# ============================================================

add_executable(storage_tests
    server/storage_tests.cpp
    server/dictionary_encoder.cpp
    server/mock_student_ingestion.cpp
)

target_link_libraries(storage_tests
    PRIVATE
        hegadb_server
        hegadb_crypto
        GTest::gtest_main
)

gtest_discover_tests(storage_tests)

# ============================================================
# Server Execution Engine Tests
# ============================================================

add_executable(execution_batch_tests
    server/execution_batch_tests.cpp
)

target_link_libraries(execution_batch_tests
    PRIVATE
        hegadb_server
        hegadb_crypto
        GTest::gtest_main
)

gtest_discover_tests(execution_batch_tests)

add_executable(execution_parameters_tests
    server/execution_parameters_tests.cpp
)

target_link_libraries(execution_parameters_tests
    PRIVATE
        hegadb_server
        hegadb_crypto
        GTest::gtest_main
)

gtest_discover_tests(execution_parameters_tests)

add_executable(query_planner_tests
    server/query_planner_tests.cpp
)

target_link_libraries(query_planner_tests
    PRIVATE
        hegadb_server
        hegadb_catalog_client
        GTest::gtest_main
)

gtest_discover_tests(query_planner_tests)

add_executable(expression_planner_tests
    server/expression_planner_tests.cpp
)

target_link_libraries(expression_planner_tests
    PRIVATE
        hegadb_server
        hegadb_protocol
        GTest::gtest_main
)

gtest_discover_tests(expression_planner_tests)

add_executable(comparison_evaluator_tests
    server/comparison_evaluator_tests.cpp
)

target_link_libraries(comparison_evaluator_tests
    PRIVATE
        hegadb_server
        hegadb_crypto
        GTest::gtest_main
)

gtest_discover_tests(comparison_evaluator_tests)

add_executable(end_to_end_execution_tests
    server/end_to_end_execution_tests.cpp
)

target_link_libraries(end_to_end_execution_tests
    PRIVATE
        hegadb_server
        hegadb_catalog_client
        hegadb_crypto
        GTest::gtest_main
)

gtest_discover_tests(end_to_end_execution_tests)

add_executable(join_query_tests
    server/join_query_tests.cpp
)

target_link_libraries(join_query_tests
    PRIVATE
        hegadb_server
        hegadb_catalog_client
        hegadb_crypto
        hegadb_protocol
        GTest::gtest_main
)

gtest_discover_tests(join_query_tests)

# ============================================================
# Server Create & Insert Tests
# ============================================================

add_executable(create_and_insert_tests
    server/create_and_insert_tests.cpp
)

target_link_libraries(create_and_insert_tests
    PRIVATE
        hegadb_server
        hegadb_catalog_client
        hegadb_client_transport
        hegadb_server_transport
        hegadb_crypto
        hegadb_protocol
        GTest::gtest_main
)

gtest_discover_tests(create_and_insert_tests)

# ============================================================
# Integration Tests
# ============================================================

add_subdirectory(integration)
