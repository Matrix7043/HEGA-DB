#include <hegadb/logging/Logger.h>

#include <spdlog/sinks/stdout_color_sinks.h>

namespace hegadb::logging {

void initialize()
{
    auto logger = spdlog::stdout_color_mt("hegadb");

    spdlog::set_default_logger(logger);

    spdlog::set_pattern(
        "[%Y-%m-%d %H:%M:%S.%e] "
        "[%^%l%$] "
        "%v");

    spdlog::set_level(
        spdlog::level::debug);
}

void shutdown()
{
    spdlog::shutdown();
}

}
