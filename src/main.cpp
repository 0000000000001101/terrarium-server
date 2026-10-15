#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

int main()
{
    // Console sink (with color support)
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);
    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

    // File sink
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/latest.log", true);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

    // Rotating file sink
    auto rotating_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/server.log", 1024*1024*5 /* 5MiB */, 3, true);
    rotating_file_sink->set_level(spdlog::level::trace);
    rotating_file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

    // Combine all sinks into a single logger
    auto logger = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list{console_sink, file_sink, rotating_file_sink});
    logger->set_level(spdlog::level::trace);

    // Register it as the default global logger
    spdlog::set_default_logger(logger);



    spdlog::set_level(spdlog::level::trace);
    spdlog::info("Hello, world!");

#ifdef TERRARIUM_DEBUG
    spdlog::trace("TERRARIUM_DEBUG");
#endif
#ifdef TERRARIUM_RELEASE
    spdlog::trace("TERRARIUM_RELEASE");
#endif

    return 0;
}
