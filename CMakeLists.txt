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


# ============================================================
# HEGADB Client
# ============================================================

add_executable(hegadb

    src/main.cpp

    ${ANTLR_GENERATED_SOURCES}
)


# ============================================================
# Include directories
# ============================================================

target_include_directories(hegadb

    PRIVATE

        ${CMAKE_CURRENT_SOURCE_DIR}/generated

        ${CMAKE_CURRENT_SOURCE_DIR}/include

        /usr/include/antlr4-runtime
)


# ============================================================
# ANTLR runtime
# ============================================================

target_link_libraries(hegadb

    PRIVATE

        antlr4-runtime
)
