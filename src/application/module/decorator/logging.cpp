#include <arba/appt/application/module/decorator/logging.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

inline namespace arba
{
namespace appt
{
inline namespace mdec // module_decorator
{
namespace private_
{

spdlog::sink_ptr logging_impl::make_console_sink()
{
    spdlog::sink_ptr console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%^%8l%$][%Y-%m-%d %H:%M:%S.%f][%n]: %v");
    return console_sink;
}

spdlog::sink_ptr logging_impl::make_file_sink(const std::filesystem::path& log_fpath)
{
    constexpr unsigned log_file_max_size = 4 * 1024 * 1024;
    constexpr unsigned max_number_of_log_files = 128;
    spdlog::sink_ptr file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_fpath.string(),
                                                                                        log_file_max_size,
                                                                                        max_number_of_log_files);
    file_sink->set_pattern("[%8l][%Y-%m-%d %H:%M:%S.%f][%n %t][%@ %!]: %v");
    return file_sink;
}

void logging_impl::initialize_logger(std::shared_ptr<spdlog::logger>& logger)
{
    spdlog::register_logger(logger);
}

void logging_impl::destroy_logger(std::shared_ptr<spdlog::logger> logger)
{
    spdlog::drop(logger->name());
}

}
}
}
}
