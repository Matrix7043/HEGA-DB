find_package(Protobuf CONFIG QUIET)
if(NOT Protobuf_FOUND)
  find_package(Protobuf REQUIRED)
endif()

find_package(gRPC CONFIG REQUIRED)

# ============================================================
# 1. Shared gRPC Stub (Generated Code)
# ============================================================
# Both the client and server need the generated Protobuf/gRPC
# code, so we compile it once here.

set(HEGADB_GRPC_PROTO_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/grpc/proto/service.proto
)

protobuf_generate(
    LANGUAGE cpp
    OUT_VAR HEGADB_GRPC_PROTO_SRCS
    PROTOS ${HEGADB_GRPC_PROTO_FILES}
    IMPORT_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/grpc/proto
        ${CMAKE_SOURCE_DIR}/protocol/proto
)

set(HEGADB_GRPC_GRPC_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.h
)

add_custom_command(
    OUTPUT ${HEGADB_GRPC_GRPC_SRCS}
    COMMAND protobuf::protoc
        "--grpc_out=${CMAKE_CURRENT_BINARY_DIR}"
        "--plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
        "-I${CMAKE_CURRENT_SOURCE_DIR}/src/grpc/proto"
        "-I${CMAKE_SOURCE_DIR}/protocol/proto"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/grpc/proto/service.proto"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/grpc/proto/service.proto"
        gRPC::grpc_cpp_plugin
)

add_library(hegadb_grpc_stub
    ${HEGADB_GRPC_PROTO_SRCS}
    ${HEGADB_GRPC_GRPC_SRCS}
)

target_include_directories(hegadb_grpc_stub PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(hegadb_grpc_stub
    PUBLIC
        hegadb_protocol
        protobuf::libprotobuf
        gRPC::grpc++
)


# ============================================================
# 2. Client Transport Library
# ============================================================
# Contains the ClientTransport interface, the GrpcClient implementation,
# and the ClientTransportFactory.

if(HEGADB_BUILD_CLIENT)

  add_library(hegadb_client_transport
    src/grpc/GrpcClient.cpp
    src/ClientTransportFactory.cpp
)

  target_include_directories(hegadb_client_transport
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

  target_link_libraries(hegadb_client_transport
    PUBLIC
        hegadb_grpc_stub
    PRIVATE
        hegadb_warnings
)

endif()


# ============================================================
# 3. Server Transport Library
# ============================================================
# Contains the ServerTransport interface, the GrpcServer implementation,
# the ServiceImpl, and the ServerTransportFactory.

if(HEGADB_BUILD_SERVER)

  add_library(hegadb_server_transport
    src/grpc/GrpcServer.cpp
    src/grpc/HEGADBServiceImpl.cpp
    src/ServerTransportFactory.cpp
)

  target_include_directories(hegadb_server_transport
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

  target_link_libraries(hegadb_server_transport
    PUBLIC
        hegadb_grpc_stub
    PRIVATE
        hegadb_warnings
)

endif()
