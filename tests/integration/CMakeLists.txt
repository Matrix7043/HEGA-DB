cmake_minimum_required(VERSION 3.20)

# ============================================================
# Integration Test: In-Memory Query Execution (Interactive REPL)
# ============================================================

add_executable(integrated_query_test
    integrated_query_test.cpp
    InMemoryStorageManager.cpp
    TestDataGenerator.cpp
)

target_link_libraries(integrated_query_test
    PRIVATE
        # Core libraries
        hegadb_catalog
        hegadb_crypto
        hegadb_types
        hegadb_logging
        hegadb_protocol
        
        # Client components
        hegadb_client_core
        
        # Server components
        hegadb_server
        
        # Execution utilities
        hegadb_execution
        
        # System libraries
        readline
        
        hegadb_warnings
)

target_compile_features(integrated_query_test PRIVATE cxx_std_20)

# Note: integrated_query_test is an interactive REPL waiting on stdin, not registered in CTest.

# ============================================================
# Integration Test: FileStorageManager
# ============================================================

add_executable(test_file_storage
    test_file_storage.cpp
)

target_link_libraries(test_file_storage
    PRIVATE
        # Core libraries
        hegadb_catalog
        hegadb_crypto
        hegadb_types
        hegadb_logging
        
        # Server components
        hegadb_server
        
        hegadb_warnings
)

target_compile_features(test_file_storage PRIVATE cxx_std_20)

add_test(
    NAME FileStorageTest
    COMMAND test_file_storage
)

# ============================================================
# Integration Test: Query Execution with FileStorage
# ============================================================

add_executable(test_query_execution
    test_query_execution.cpp
)

target_link_libraries(test_query_execution
    PRIVATE
        # Core libraries
        hegadb_catalog
        hegadb_crypto
        hegadb_types
        hegadb_logging
        hegadb_protocol
        
        # Client components
        hegadb_client_core
        
        # Server components
        hegadb_server
        
        # Execution utilities
        hegadb_execution
        
        hegadb_warnings
)

target_compile_features(test_query_execution PRIVATE cxx_std_20)

add_test(
    NAME QueryExecutionTest
    COMMAND test_query_execution
)

# ============================================================
# Diagnostic: Column slot inspection
# ============================================================
add_executable(diag_columns diag_columns.cpp)
target_link_libraries(diag_columns PRIVATE hegadb_catalog hegadb_crypto hegadb_server hegadb_warnings)
target_compile_features(diag_columns PRIVATE cxx_std_20)

# ============================================================
# Integration Test: Query types (projection, WHERE, AND/OR/NOT)
# ============================================================
add_executable(test_query_types test_query_types.cpp)
target_link_libraries(test_query_types
    PRIVATE
        hegadb_catalog
        hegadb_crypto
        hegadb_types
        hegadb_logging
        hegadb_protocol
        hegadb_client_core
        hegadb_server
        hegadb_execution
        hegadb_warnings
)
target_compile_features(test_query_types PRIVATE cxx_std_20)

add_test(
    NAME QueryTypesTest
    COMMAND test_query_types
)

# ============================================================
# Integration Test: Full gRPC transport end-to-end
# (GrpcClient → wire → GrpcServer → QueryExecutor → ResultEncoder
#  → wire → ResultDecryptor)
# Requires the EMPLOYEES table on disk (hegadb_setup --init-database).
# Skips gracefully when the data directory is absent.
# ============================================================
add_executable(test_grpc_transport test_grpc_transport.cpp)
target_link_libraries(test_grpc_transport
    PRIVATE
        hegadb_server_transport    # GrpcServer + HEGADBServiceImpl
        hegadb_client_transport    # GrpcClient
        hegadb_catalog
        hegadb_crypto
        hegadb_types
        hegadb_logging
        hegadb_protocol
        hegadb_client_core         # SQLParser, QueryPreparer, ExecuteQueryEncoder
        hegadb_server              # QueryExecutor, ExecuteQueryReceiver, ResultEncoder
        hegadb_execution           # ResultDecryptor, ResultFormatter
        hegadb_warnings
)
target_compile_features(test_grpc_transport PRIVATE cxx_std_20)

add_test(
    NAME GrpcTransportE2ETest
    COMMAND test_grpc_transport
)

# ============================================================
# CTest Fixture: Automatically initialize database & keys
# before running integration tests that require EMPLOYEES data
# ============================================================
add_test(
    NAME SetupTestDatabase
    COMMAND hegadb_setup --init-database
)
set_tests_properties(SetupTestDatabase PROPERTIES FIXTURES_SETUP HegadbDataFixture)

set_tests_properties(
    GrpcTransportE2ETest
    QueryExecutionTest
    FileStorageTest
    QueryTypesTest
    PROPERTIES FIXTURES_REQUIRED HegadbDataFixture
)

# Diagnostic: wire byte inspection
add_executable(debug_wire_bytes debug_wire_bytes.cpp)
target_link_libraries(debug_wire_bytes PRIVATE
    hegadb_server_transport hegadb_client_transport
    hegadb_catalog hegadb_crypto hegadb_types hegadb_logging
    hegadb_protocol hegadb_client_core hegadb_server
    hegadb_warnings)
target_compile_features(debug_wire_bytes PRIVATE cxx_std_20)
