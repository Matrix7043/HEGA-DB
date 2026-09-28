cmake_minimum_required(VERSION 3.20)

# Find OpenFHE (required for FHE operations)
find_package(OpenFHE REQUIRED)

# ============================================================
# CUDA Option
# ============================================================
option(HEGADB_ENABLE_CUDA "Enable CUDA features" OFF)

# ============================================================
# Server Library
# ============================================================
add_library(hegadb_server
    # API Layer
    src/api/ExecuteQueryReceiver.cpp

    # Execution Layer
    src/execution/QueryExecutor.cpp
    src/execution/ExecutionEngine.cpp
    src/execution/ExecutionContext.cpp
    src/execution/ExecutionPlan.cpp
    src/execution/ExecutionResult.cpp
    src/execution/ExecutionBatch.cpp
    src/execution/EncryptedMask.cpp
    src/execution/ExecutionParameters.cpp

    # Planning Layer
    src/planner/QueryPlanner.cpp
    src/planner/ExpressionPlanner.cpp

    # Operators
    src/operators/TableScanOperator.cpp
    src/operators/FilterOperator.cpp
    src/operators/ProjectionOperator.cpp

    # Evaluators
    src/evaluator/ExpressionEvaluator.cpp
    src/evaluator/PredicateEvaluator.cpp
    src/evaluator/ComparisonEvaluator.cpp
    src/evaluator/LogicalEvaluator.cpp

    # Result Handling
    src/result/RowsAndMaskResultStrategy.cpp
    src/result/MaskedRowsResultStrategy.cpp
    src/result/ResultEncoder.cpp

    # Storage Adapter Layer
    src/storage/StorageManager.cpp
    src/storage/EncryptedTable.cpp
    src/storage/EncryptedBatch.cpp

    # Physical Storage Layer (Existing)
    src/storage/pinned_memory_pool.cpp
    src/storage/table_manifest.cpp
    src/storage/segment_writer.cpp
    src/storage/segment_reader.cpp
)

# ============================================================
# Compiler Definitions
# ============================================================
if(HEGADB_ENABLE_CUDA)
    target_compile_definitions(hegadb_server PUBLIC HEGADB_ENABLE_CUDA)
endif()

# ============================================================
# Compiler Options - Suppress OpenFHE warnings
# ============================================================
target_compile_options(hegadb_server PRIVATE
    -w  # Suppress all warnings
)

# ============================================================
# Include Directories
# ============================================================
target_include_directories(hegadb_server
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${OpenFHE_INCLUDE}
        ${OpenFHE_INCLUDE}/third-party/include
        ${OpenFHE_INCLUDE}/core
        ${OpenFHE_INCLUDE}/pke
        ${OpenFHE_INCLUDE}/binfhe
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ============================================================
# Link Libraries
# ============================================================
target_link_libraries(hegadb_server
    PUBLIC
        # External dependencies
        OPENFHEpke
        OPENFHEbinfhe
        OPENFHEcore

        # Internal modules
        hegadb_catalog
        hegadb_crypto
        hegadb_protocol
        hegadb_types
        hegadb_logging

    PRIVATE
        # Transport layer (for gRPC service integration)
        hegadb_server_transport

        # Operations (FHE operations if needed)
        hegadb_operations

        # Warnings
        hegadb_warnings
)

# ============================================================
# Server Executable
# ============================================================
add_executable(hegadb_server_app
    src/main.cpp
)

target_link_libraries(hegadb_server_app
    PRIVATE
        hegadb_server
        hegadb_server_transport
        hegadb_warnings
)

# Set executable name
set_target_properties(hegadb_server_app
    PROPERTIES
        OUTPUT_NAME hegadb_server
)

# ============================================================
# Installation Rules (Optional)
# ============================================================
install(TARGETS hegadb_server hegadb_server_app
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
