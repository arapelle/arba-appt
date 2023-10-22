#include <arba/appt/application/decorator/logging.hpp>
#include <arba/appt/application/program_args.hpp>
#include <arba/appt/util/format_time_point.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

inline namespace arba
{
namespace appt
{
inline namespace adec // application_decorator
{
namespace private_
{

std::filesystem::path logging_impl::make_log_directory_path(const program_args& args)
{
    std::filesystem::path app_log_dirname = args.empty() ? std::filesystem::path("application") : args.program_stem();
    return std::filesystem::temp_directory_path() / app_log_dirname / "log" / format_filename_Ymd_HMS_mcs();
}

std::string logging_impl::make_logger_name(const program_args& args)
{
    return args.empty() ? "application" : args.program_stem().generic_string();
}

spdlog::sink_ptr logging_impl::make_console_sink()
{
    spdlog::sink_ptr console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%^%8l%$][%Y-%m-%d %H:%M:%S.%f][%n]: %v");
    return console_sink;
}

spdlog::sink_ptr logging_impl::make_file_sink(const std::filesystem::path& log_fpath)
{
    const unsigned max_size = 4 * 1024 * 1024;
    const unsigned max_files = 128;
    spdlog::sink_ptr file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_fpath.string(),
                                                                                        max_size, max_files);
    file_sink->set_pattern("[%8l][%Y-%m-%d %H:%M:%S.%f][%n %t][%@ %!]: %v");
    return file_sink;
}

void logging_impl::initialize_logger(std::shared_ptr<spdlog::logger>& logger)
{
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
}

void logging_impl::destroy_logger(std::shared_ptr<spdlog::logger> logger)
{
    spdlog::drop(logger->name());
}

}
}
}
}
