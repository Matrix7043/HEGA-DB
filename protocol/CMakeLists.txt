find_package(Protobuf REQUIRED)

set(HEGADB_PROTOCOL_PROTO_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/common.proto
    ${CMAKE_CURRENT_SOURCE_DIR}/proto/query.proto
)

protobuf_generate(
    LANGUAGE cpp
    OUT_VAR HEGADB_PROTOCOL_PROTO_SRCS
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/proto
    PROTOS ${HEGADB_PROTOCOL_PROTO_FILES}
)

add_library(hegadb_protocol
    ${HEGADB_PROTOCOL_PROTO_SRCS}
)

target_link_libraries(hegadb_protocol
    PUBLIC
        protobuf::libprotobuf
)

target_include_directories(hegadb_protocol
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)
