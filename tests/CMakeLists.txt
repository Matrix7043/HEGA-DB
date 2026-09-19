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


# ============================================================
# Catalog Tests
# ============================================================

add_executable(catalog_tests
    catalog/catalog_tests.cpp
)

target_link_libraries(catalog_tests
    PRIVATE
        hegadb_catalog
)

add_test(
    NAME catalog_tests
    COMMAND catalog_tests
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

add_executable(crypto_edge_cases_tests
    crypto/crypto_edge_cases_tests.cpp
)

target_link_libraries(crypto_edge_cases_tests
    PRIVATE
        hegadb_crypto
)

add_test(
    NAME crypto_edge_cases_tests
    COMMAND crypto_edge_cases_tests
)


# ============================================================
# gRPC Tests
# ============================================================

find_package(GTest CONFIG REQUIRED)

add_executable(hegadb_grpc_tests
  grpc/grpc_tests.cpp
)

target_link_libraries(hegadb_grpc_tests
  hegadb_grpc_client
  hegadb_grpc_server
  GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(hegadb_grpc_tests)
