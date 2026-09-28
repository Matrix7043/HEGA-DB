#ifndef HEGADB_LOGGING_LOGGING_CONFIG_H
#define HEGADB_LOGGING_LOGGING_CONFIG_H

#include <cstddef>
#include <spdlog/spdlog.h>
#include <string>

namespace hegadb::logging {

struct LoggingConfig {

    spdlog::level::level_enum level {
        spdlog::level::info
    };

    bool console_enabled { true };
    bool file_enabled { false };

    std::string file_path { "hegadb.log" };
    std::size_t max_file_size { 10 * 1024 * 1024 };
    std::size_t max_files { 3 };
};

}

#endif
