cmake_minimum_required(VERSION 3.20)

project(HEGADB
    VERSION 0.4.0
    LANGUAGES CXX
)

# ============================================================
# C++ configuration
# ============================================================

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ============================================================
# Testing
# ============================================================

include(CTest)

# ============================================================
# ANTLR4 C++ Runtime
# ============================================================

find_package(antlr4-runtime CONFIG REQUIRED)

set(ANTLR4_RUNTIME_INCLUDE_DIR
    "/usr/include/antlr4-runtime"
)


# ============================================================
# Generated ANTLR sources
# ============================================================

set(ANTLR_GENERATED_SOURCES

    ${CMAKE_CURRENT_SOURCE_DIR}/generated/HEGADBLexer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/generated/HEGADBParser.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/generated/HEGADBParserBaseVisitor.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/generated/HEGADBParserBaseListener.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/generated/HEGADBParserVisitor.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/generated/HEGADBParserListener.cpp
)


# ============================================================
# HEGADB Core Library
# ============================================================

add_library(hegadb_core

    src/SQLParser.cpp
    src/SyntaxErrorListener.cpp

    src/ast/ASTPrinter.cpp
    src/ast/ASTBuilder.cpp
    src/ast/ASTParameterizer.cpp

    src/catalog/TableSchema.cpp
    src/catalog/CatalogSnapshot.cpp
    src/catalog/InMemoryCatalog.cpp

    src/semantic/SemanticValidator.cpp

    src/crypto/MockCiphertext.cpp
    src/crypto/MockEncryption.cpp
    src/crypto/Plaintext.cpp

    src/preparation/LiteralExtrator.cpp
    src/preparation/EncryptedParameter.cpp
    src/preparation/PreparedQuery.cpp
    src/preparation/QueryPreparer.cpp

    ${ANTLR_GENERATED_SOURCES}
)

target_include_directories(hegadb_core

    PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}/include

    PRIVATE

        ${CMAKE_CURRENT_SOURCE_DIR}/generated
        ${ANTLR4_RUNTIME_INCLUDE_DIR}
)

target_link_libraries(hegadb_core

    PUBLIC

        antlr4-runtime
)


# ============================================================
# GNU Readline
# ============================================================

find_path(
    READLINE_INCLUDE_DIR
    readline/readline.h
)

find_library(
    READLINE_LIBRARY
    readline
)

if(NOT READLINE_INCLUDE_DIR OR NOT READLINE_LIBRARY)

  message(FATAL_ERROR "GNU Readline not found")

endif()


# ============================================================
# HEGADB Client
# ============================================================

add_executable(hegadb_client

    src/main.cpp
)

target_include_directories(hegadb_client

    PRIVATE

        ${READLINE_INCLUDE_DIR}
)

target_link_libraries(hegadb_client

    PRIVATE

        hegadb_core
        ${READLINE_LIBRARY}
)

# ============================================================
# Tests
# ============================================================

include(CTest)

if(BUILD_TESTING)

  add_executable(parser_tests
        tests/parser_tests.cpp
    )

  target_link_libraries(parser_tests
        PRIVATE
            hegadb_core
    )

  add_test(
        NAME parser_tests
        COMMAND parser_tests
    )


  add_executable(ast_tests
        tests/ast_tests.cpp
    )

  target_link_libraries(ast_tests
        PRIVATE
            hegadb_core
    )

  add_test(
        NAME ast_tests
        COMMAND ast_tests
    )


  add_executable(ast_builder_tests
        tests/ast_builder_tests.cpp
    )

  target_include_directories(ast_builder_tests
        PRIVATE
            ${CMAKE_SOURCE_DIR}/generated
            ${ANTLR4_RUNTIME_INCLUDE_DIR}
    )

  target_link_libraries(ast_builder_tests
        PRIVATE
            hegadb_core
    )

  add_test(
        NAME ast_builder_tests
        COMMAND ast_builder_tests
    )


  add_executable(catalog_tests
        tests/catalog_tests.cpp
    )

  target_link_libraries(catalog_tests
        PRIVATE
            hegadb_core
    )

  add_test(
        NAME catalog_tests
        COMMAND catalog_tests
    )

  add_executable(semantic_validator_tests
    tests/semantic_validator_tests.cpp
  )

  target_link_libraries(semantic_validator_tests
    PRIVATE
      hegadb_core
  )

  add_test(
    NAME semantic_validator_tests
    COMMAND semantic_validator_tests
  )

  add_executable(literal_extractor_tests
    tests/literal_extractor_tests.cpp
  )

  target_link_libraries(literal_extractor_tests
      PRIVATE
          hegadb_core
  )

  add_test(
      NAME literal_extractor_tests
      COMMAND literal_extractor_tests
  )

  add_executable(query_preparer_tests
    tests/query_preparer_tests.cpp
  )

  target_link_libraries(query_preparer_tests
    PRIVATE
      hegadb_core
  )

  add_test(
    NAME query_preparer_tests
    COMMAND query_preparer_tests
  )

  add_executable(ast_parameterizer_tests
    tests/ast_parameterizer_tests.cpp
  )

  target_link_libraries(ast_parameterizer_tests
    PRIVATE
      hegadb_core
  )

  add_test(
    NAME ast_parameterizer_tests
    COMMAND ast_parameterizer_tests
  )

endif()
