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

# ============================================================
# Components
#
# Each executable can be built on its own (standalone) or together:
#
#   catalog : catalog_microservice    (gRPC, protobuf)
#   server  : hegadb_server_app       (gRPC, protobuf, spdlog, OpenFHE)
#   client  : hegadb_client           (gRPC, protobuf, spdlog, OpenFHE,
#                                      ANTLR4, readline)
#
# Standalone examples:
#
#   cmake -S . -B build-catalog -DHEGADB_BUILD_CATALOG=ON \
#         -DHEGADB_BUILD_SERVER=OFF -DHEGADB_BUILD_CLIENT=OFF -DHEGADB_BUILD_TESTS=OFF
#   cmake -S . -B build-server  -DHEGADB_BUILD_SERVER=ON \
#         -DHEGADB_BUILD_CATALOG=OFF -DHEGADB_BUILD_CLIENT=OFF -DHEGADB_BUILD_TESTS=OFF
#   cmake -S . -B build-client  -DHEGADB_BUILD_CLIENT=ON \
#         -DHEGADB_BUILD_CATALOG=OFF -DHEGADB_BUILD_SERVER=OFF -DHEGADB_BUILD_TESTS=OFF
# ============================================================

option(HEGADB_BUILD_CATALOG "Build HEGADB catalog service" ON)
option(HEGADB_BUILD_CLIENT "Build HEGADB client" ON)
option(HEGADB_BUILD_SERVER "Build HEGADB server" ON)
option(HEGADB_BUILD_TESTS "Build HEGADB tests" ON)

if(NOT (HEGADB_BUILD_CATALOG OR HEGADB_BUILD_CLIENT OR HEGADB_BUILD_SERVER))
  message(FATAL_ERROR
    "Nothing to build: enable at least one of HEGADB_BUILD_CATALOG, "
    "HEGADB_BUILD_CLIENT or HEGADB_BUILD_SERVER")
endif()

# The tests link against the catalog, server and client libraries.
if(HEGADB_BUILD_TESTS
   AND NOT (HEGADB_BUILD_CATALOG AND HEGADB_BUILD_CLIENT AND HEGADB_BUILD_SERVER))
  message(FATAL_ERROR
    "HEGADB_BUILD_TESTS=ON requires HEGADB_BUILD_CATALOG, HEGADB_BUILD_CLIENT "
    "and HEGADB_BUILD_SERVER to be ON. Use -DHEGADB_BUILD_TESTS=OFF for a "
    "standalone build.")
endif()

message(STATUS "HEGADB components: "
  "catalog=${HEGADB_BUILD_CATALOG} "
  "server=${HEGADB_BUILD_SERVER} "
  "client=${HEGADB_BUILD_CLIENT} "
  "tests=${HEGADB_BUILD_TESTS}")

# Header-only libraries, used by every component.
add_subdirectory(operations)
add_subdirectory(types)

# logging (spdlog) and crypto (OpenFHE) are only used by the client and the
# server. The catalog service needs neither, so a catalog-only build does not
# require them to be installed.
if(HEGADB_BUILD_CLIENT OR HEGADB_BUILD_SERVER)
  add_subdirectory(logging)
  add_subdirectory(crypto)
endif()

# Shared by all components. catalog/ and transport/ decide internally
# which of their libraries to define based on the options above.
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
