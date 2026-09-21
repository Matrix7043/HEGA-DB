add_library(hegadb_server
    src/ExecuteQueryReceiver.cpp
    src/QueryExecutor.cpp
)

target_include_directories(hegadb_server
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(hegadb_server
    PUBLIC
        hegadb_protocol
        hegadb_server_transport
        hegadb_catalog
        hegadb_operations
        hegadb_types
)

add_executable(hegadb_server_app src/main.cpp)

target_link_libraries(hegadb_server_app PRIVATE hegadb_server)
