option(HEGADB_ENABLE_CUDA "Enable CUDA features" OFF)

add_library(hegadb_server
    src/ExecuteQueryReceiver.cpp
    src/QueryExecutor.cpp
    src/storage/pinned_memory_pool.cpp
    src/storage/table_manifest.cpp
    src/storage/segment_writer.cpp
    src/storage/segment_reader.cpp
)

if(HEGADB_ENABLE_CUDA)
    target_compile_definitions(hegadb_server PUBLIC HEGADB_ENABLE_CUDA)
endif()

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
        hegadb_crypto
)

add_executable(hegadb_server_app src/main.cpp)

target_link_libraries(hegadb_server_app PRIVATE hegadb_server)
