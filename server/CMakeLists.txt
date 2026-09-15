add_library(hegadb_server INTERFACE)

target_include_directories(hegadb_server
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
