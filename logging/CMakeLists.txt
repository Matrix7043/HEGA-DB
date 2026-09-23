find_package(spdlog CONFIG REQUIRED)

add_library(hegadb_logging
  src/Logger.cpp
)

target_include_directories(hegadb_logging
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(hegadb_logging
  PUBLIC
    spdlog::spdlog
)

target_compile_features(hegadb_logging
  PUBLIC
    cxx_std_20
)
