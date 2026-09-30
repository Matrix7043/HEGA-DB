#ifndef LOGGER_H
#define LOGGER_H

#include <hegadb/logging/LoggingConfig.h>
#include <utility>

#include <spdlog/spdlog.h>

namespace hegadb::logging {

void initialize(LoggingConfig const& config);

void shutdown();

spdlog::level::level_enum
logLevelEnumParser(std::string const& str);

template<typename... Args>
void trace(
    spdlog::format_string_t<Args...> format,
    Args&&... args)
{
    spdlog::trace(
        format,
        std::forward<Args>(args)...);
}

template<typename... Args>
void debug(
    spdlog::format_string_t<Args...> format,
    Args&&... args)
{
    spdlog::debug(
        format,
        std::forward<Args>(args)...);
}

template<typename... Args>
void info(
    spdlog::format_string_t<Args...> format,
    Args&&... args)
{
    spdlog::info(
        format,
        std::forward<Args>(args)...);
}

template<typename... Args>
void warn(
    spdlog::format_string_t<Args...> format,
    Args&&... args)
{
    spdlog::warn(
        format,
        std::forward<Args>(args)...);
}

template<typename... Args>
void error(
    spdlog::format_string_t<Args...> format,
    Args&&... args)
{
    spdlog::error(
        format,
        std::forward<Args>(args)...);
}

template<typename... Args>
void critical(
    spdlog::format_string_t<Args...> format,
    Args&&... args)
{
    spdlog::critical(
        format,
        std::forward<Args>(args)...);
}

}

#endif
