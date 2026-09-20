find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(HEGADB_GRPC_PROTO_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/service.proto
)

# 1. Generate standard C++ message files (.pb.cc / .pb.h)
protobuf_generate(
    LANGUAGE cpp
    OUT_VAR HEGADB_GRPC_PROTO_SRCS
    PROTOS ${HEGADB_GRPC_PROTO_FILES}
    IMPORT_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/proto
      ${CMAKE_SOURCE_DIR}/protocol/proto
)

# 2. Define the expected gRPC output files
set(HEGADB_GRPC_GRPC_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.h
)

# 3. Generate gRPC service files (.grpc.pb.cc / .grpc.pb.h) directly using protoc
# 3. Generate gRPC service files (.grpc.pb.cc / .grpc.pb.h) directly using protoc
add_custom_command(
    OUTPUT ${HEGADB_GRPC_GRPC_SRCS}
    COMMAND protobuf::protoc
    "--grpc_out=${CMAKE_CURRENT_BINARY_DIR}"
    "--plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
    "-I${CMAKE_CURRENT_SOURCE_DIR}/proto"
    "-I${CMAKE_SOURCE_DIR}/protocol/proto"
    "${CMAKE_CURRENT_SOURCE_DIR}/proto/service.proto"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/proto/service.proto" gRPC::grpc_cpp_plugin
)

add_library(hegadb_grpc
    ${HEGADB_GRPC_PROTO_SRCS}
    ${HEGADB_GRPC_GRPC_SRCS}
)

target_link_libraries(hegadb_grpc
    PUBLIC
        hegadb_protocol
        protobuf::libprotobuf
        gRPC::grpc++
)

target_include_directories(hegadb_grpc
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(hegadb_grpc_server
  server/HEGADBServiceImpl.cpp
  server/GrpcServer.cpp
)

target_link_libraries(hegadb_grpc_server
  PUBLIC
    hegadb_transport
    hegadb_grpc
)

target_include_directories(hegadb_grpc_server
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/server
)

add_library(hegadb_grpc_client
  client/GrpcClient.cpp
)

target_link_libraries(hegadb_grpc_client
  PUBLIC
    hegadb_transport
    hegadb_grpc
)

target_include_directories(hegadb_grpc_client
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/client
)
