add_library(hegadb_server INTERFACE)

target_link_libraries(hegadb_server
    PRIVATE
        hegadb_protocol
)

target_include_directories(hegadb_server
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
