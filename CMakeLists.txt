cmake_minimum_required(VERSION 3.20)

project(HEGADB
    VERSION 0.1.0
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
# ANTLR4 C++ Runtime
# ============================================================

find_package(antlr4-runtime CONFIG REQUIRED)


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

set(ANTLR4_RUNTIME_INCLUDE_DIR
  "/usr/include/antlr4-runtime"
)

add_library(hegadb_core

    src/SQLParser.cpp

    src/SyntaxErrorListener.cpp

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

find_path(READLINE_INCLUDE_DIR

    readline/readline.h

)

find_library(READLINE_LIBRARY

    readline
)

if(NOT READLINE_INCLUDE_DIR OR NOT READLINE_LIBRARY)
  message(FATAL_ERROR "GNU Readline not found")
endif()

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
# Parser tests
# ============================================================

add_executable(parser_tests

    tests/parser_tests.cpp
)


target_link_libraries(parser_tests

    PRIVATE

        hegadb_core
)

include(CTest)

if(BUILD_TESTING)

  add_test(
  NAME parser_tests
  COMMAND parser_tests
  )

endif()
