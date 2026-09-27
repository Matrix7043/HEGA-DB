#include <hegadb/logging/Logger.h>

#include <memory>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <spdlog/spdlog.h>

namespace hegadb::logging {

void initialize(LoggingConfig const& config)
{
    std::vector<spdlog::sink_ptr> sinks;

    if (config.console_enabled) {
        sinks.push_back(std::make_shared<
            spdlog::sinks::stdout_color_sink_mt>());
    }

    if (config.file_enabled) {
        sinks.push_back(std::make_shared<
            spdlog::sinks::rotating_file_sink_mt>(config.file_path,
            config.max_file_size,
            config.max_files));
    }

    auto logger = std::make_shared<
        spdlog::logger>("hegadb", sinks.begin(), sinks.end());

    logger->set_level(config.level);

    logger->set_pattern(
        "[%Y-%m-%d %H:%M:%S.%e] "
        "[%^%l%$] "
        "%v");
    // Flush after INFO and above
    logger->flush_on(spdlog::level::info);
    spdlog::set_default_logger(std::move(logger));
    spdlog::set_level(config.level);
}

spdlog::level::level_enum logLevelEnumParser(std::string const& str)
{
    static std::unordered_map<std::string, spdlog::level::level_enum> const log_level_map = {
        { "info", spdlog::level::info },
        { "critical", spdlog::level::critical },
        { "warn", spdlog::level::warn },
        { "error", spdlog::level::err },
        { "trace", spdlog::level::trace },
        { "off", spdlog::level::off }
    };

    auto it = log_level_map.find(str);
    if (it == log_level_map.end()) {
        throw std::invalid_argument("Unknown log-level string " + str);
    }
    return it->second;
}

void shutdown()
{
    spdlog::shutdown();
}

}
