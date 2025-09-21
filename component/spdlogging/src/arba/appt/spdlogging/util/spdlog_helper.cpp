#include <arba/appt/spdlogging/util/spdlog_helper.hpp>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

inline namespace arba
{
namespace appt
{

spdlog::sink_ptr spdlog_helper::make_console_sink_mt(std::string_view log_pattern)
{
    std::shared_ptr sink_ptr = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    initialize_console_sink(sink_ptr, log_pattern);
    return sink_ptr;
}

spdlog::sink_ptr spdlog_helper::make_console_sink_st(std::string_view log_pattern)
{
    std::shared_ptr sink_ptr = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    initialize_console_sink(sink_ptr, log_pattern);
    return sink_ptr;
}

spdlog::sink_ptr spdlog_helper::make_rotating_file_sink_mt(const std::filesystem::path& log_file,
                                                           std::string_view log_pattern, std::size_t max_size,
                                                           std::size_t max_nb_log_files)
{
    std::shared_ptr sink_ptr =
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file.string(), max_size, max_nb_log_files);
    initialize_rotating_file_sink(sink_ptr, log_pattern);
    return sink_ptr;
}

spdlog::sink_ptr spdlog_helper::make_rotating_file_sink_st(const std::filesystem::path& log_file,
                                                           std::string_view log_pattern, std::size_t max_size,
                                                           std::size_t max_nb_log_files)
{
    std::shared_ptr sink_ptr =
        std::make_shared<spdlog::sinks::rotating_file_sink_st>(log_file.string(), max_size, max_nb_log_files);
    initialize_rotating_file_sink(sink_ptr, log_pattern);
    return sink_ptr;
}

void spdlog_helper::initialize_console_sink(spdlog::sink_ptr sink_ptr, std::string_view log_pattern)
{
    sink_ptr->set_pattern(std::string(log_pattern));
}

void spdlog_helper::initialize_rotating_file_sink(spdlog::sink_ptr sink_ptr, std::string_view log_pattern)
{
    sink_ptr->set_pattern(std::string(log_pattern));
}

} // namespace appt
} // namespace arba
