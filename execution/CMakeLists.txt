cmake_minimum_required(VERSION 3.20)

project(hegadb_execution VERSION 0.7.0 LANGUAGES CXX)

# ============================================================
# Find GNU Readline
# ============================================================

find_path(READLINE_INCLUDE_DIR readline/readline.h)
find_library(READLINE_LIBRARY readline)

if(NOT READLINE_INCLUDE_DIR OR NOT READLINE_LIBRARY)
    message(FATAL_ERROR "GNU Readline not found")
endif()

# ============================================================
# Execution Library
# ============================================================

add_library(hegadb_execution STATIC
    src/LocalExecutor.cpp
    src/DataLoader.cpp
    src/ResultDecryptor.cpp
    src/ResultFormatter.cpp
    src/ExecutionREPL.cpp
    src/CatalogLoader.cpp
)

target_include_directories(hegadb_execution
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${READLINE_INCLUDE_DIR}
)

target_link_libraries(hegadb_execution
    PUBLIC
        # Internal modules
        hegadb_catalog
        hegadb_crypto
        hegadb_types
        hegadb_logging
        hegadb_protocol
        
        # Client components
        hegadb_client_core
        
        # Server components
        hegadb_server
        
        # System libraries
        ${READLINE_LIBRARY}
    PRIVATE
        hegadb_warnings
)

target_compile_features(hegadb_execution PUBLIC cxx_std_20)

# ============================================================
# Execution REPL Executable
# ============================================================

add_executable(hegadb_repl
    src/repl_main.cpp
)

target_link_libraries(hegadb_repl
    PRIVATE
        hegadb_execution
        hegadb_logging
)

target_compile_features(hegadb_repl PRIVATE cxx_std_20)
