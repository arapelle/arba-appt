#include <arba/appt/util/logger_helper.hpp>
#include <arba/appt/util/format_time_point.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

inline namespace arba
{
namespace appt
{

std::filesystem::path logger_helper::log_dir(const std::filesystem::path& log_dirname)
{
    return std::filesystem::temp_directory_path()/log_dirname/"log"/format_filename_Ymd_HMS_mcs();
}

spdlog::sink_ptr logger_helper::create_console_sink()
{
    std::shared_ptr console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%^%8l%$][%Y-%m-%d %H:%M:%S.%f][%n]: %v");
    return console_sink;
}

spdlog::sink_ptr logger_helper::create_file_sink(const std::filesystem::path &log_file, std::size_t max_size, std::size_t max_files)
{
    std::shared_ptr file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file.string(), max_size, max_files);
    file_sink->set_pattern("[%8l][%Y-%m-%d %H:%M:%S.%f][%n %t][%@ %!]: %v");
    return file_sink;
}

}
}
