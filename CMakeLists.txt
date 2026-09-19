cmake_minimum_required(VERSION 3.20)

project(HEGADB
    VERSION 0.5.0
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(HEGADB_BUILD_CLIENT "Build HEGADB client" ON)
option(HEGADB_BUILD_SERVER "Build HEGADB server" OFF)
option(HEGADB_BUILD_TESTS "Build HEGADB tests" ON)

add_subdirectory(operations)
add_subdirectory(types)
add_subdirectory(crypto)
add_subdirectory(catalog)
add_subdirectory(protocol)
add_subdirectory(transport)
add_subdirectory(transport/grpc)

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
