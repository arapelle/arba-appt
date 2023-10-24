#pragma once

#include <spdlog/common.h>
#include <filesystem>

inline namespace arba
{
namespace appt
{

class logger_make_helper
{
public:
    static constexpr std::string_view default_console_log_pattern = "[%^%8l%$][%Y-%m-%d %H:%M:%S.%f][%n]: %v";

    static spdlog::sink_ptr make_console_sink_mt(std::string_view log_pattern = default_console_log_pattern);
    static spdlog::sink_ptr make_console_sink_st(std::string_view log_pattern = default_console_log_pattern);

    static void initialize_console_sink(spdlog::sink_ptr sink_ptr,
                                        std::string_view log_pattern = default_console_log_pattern);

    static constexpr std::string_view default_file_log_pattern = "[%8l][%Y-%m-%d %H:%M:%S.%f][%n %t][%@ %!]: %v";
    static constexpr unsigned default_rotating_file_max_size = 4 * 1024 * 1024;
    static constexpr unsigned default_max_number_of_rotating_files = 128;

    static spdlog::sink_ptr make_rotating_file_sink_mt(const std::filesystem::path& log_file,
                                                       std::string_view log_pattern = default_file_log_pattern,
                                                       std::size_t max_size = default_rotating_file_max_size,
                                                       std::size_t max_nb_log_files = default_max_number_of_rotating_files);
    static spdlog::sink_ptr make_rotating_file_sink_st(const std::filesystem::path& log_file,
                                                       std::string_view log_pattern = default_file_log_pattern,
                                                       std::size_t max_size = default_rotating_file_max_size,
                                                       std::size_t max_nb_log_files = default_max_number_of_rotating_files);

    static void initialize_rotating_file_sink(spdlog::sink_ptr sink_ptr,
                                              std::string_view log_pattern = default_file_log_pattern);

    ~logger_make_helper() = delete;
};

}
}
