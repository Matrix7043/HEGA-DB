find_package(Protobuf REQUIRED)

set(PROTO_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/proto/common.proto"
    "${CMAKE_CURRENT_SOURCE_DIR}/proto/query.proto"
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_library(hegadb_protocol STATIC
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

target_include_directories(hegadb_protocol
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/proto
)

target_link_libraries(hegadb_protocol
    PUBLIC
        protobuf::libprotobuf
)
