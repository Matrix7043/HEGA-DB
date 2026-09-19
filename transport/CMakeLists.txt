add_library(hegadb_transport INTERFACE)

target_include_directories(hegadb_transport
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(hegadb_transport
    INTERFACE
        hegadb_protocol
)
