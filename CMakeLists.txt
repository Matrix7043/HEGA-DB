cmake_minimum_required(VERSION 3.20)

project(HEGADB
    VERSION 0.7.0
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Compiler Warnings
# ============================================================

add_library(hegadb_warnings INTERFACE)

if(MSVC)

  target_compile_options(hegadb_warnings
        INTERFACE
            /W4
            /permissive-
    )

else()

  target_compile_options(hegadb_warnings
        INTERFACE
            -Wall
            -Wextra
            -Wpedantic
    )

endif()

option(HEGADB_BUILD_CLIENT "Build HEGADB client" ON)
option(HEGADB_BUILD_SERVER "Build HEGADB server" ON)
option(HEGADB_BUILD_TESTS "Build HEGADB tests" ON)

add_subdirectory(operations)
add_subdirectory(types)
add_subdirectory(crypto)
add_subdirectory(catalog)
add_subdirectory(protocol)
add_subdirectory(transport)

if(HEGADB_BUILD_CLIENT)
  add_subdirectory(client)
endif()

if(HEGADB_BUILD_SERVER)
  add_subdirectory(server)
endif()

if(HEGADB_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
