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
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/HEGADBParserBaseListener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/HEGADBParserBaseVisitor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/HEGADBParserListener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/HEGADBParserVisitor.cpp
)


# ============================================================
# HEGADB Client Core
# ============================================================

add_library(hegadb_client_core

    src/SQLParser.cpp
    src/SyntaxErrorListener.cpp

    src/ast/ASTBuilder.cpp
    src/ast/ASTParameterizer.cpp
    src/ast/ASTPrinter.cpp

    src/preparation/EncryptedParameter.cpp
    src/preparation/LiteralExtractor.cpp
    src/preparation/PreparedQuery.cpp
    src/preparation/QueryPreparer.cpp

    src/semantic/SemanticValidator.cpp

    ${ANTLR_GENERATED_SOURCES}
)


# ============================================================
# Include directories
# ============================================================

target_include_directories(hegadb_client_core

    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/generated
        ${ANTLR4_RUNTIME_INCLUDE_DIR}
)


# ============================================================
# Client Core Dependencies
# ============================================================

target_link_libraries(hegadb_client_core

    PUBLIC
        hegadb_operations
        hegadb_types
        hegadb_crypto
        hegadb_catalog
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
# HEGADB Client Executable
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
        hegadb_client_core
        ${READLINE_LIBRARY}
)
