#pragma once

#include <spdlog/sinks/sink.h>
#include <filesystem>

inline namespace arba
{
namespace appt
{

class logger_helper
{
public:
    ~logger_helper() = delete;
    static std::filesystem::path log_dir(const std::string& name);
    static spdlog::sink_ptr create_console_sink();
    static spdlog::sink_ptr create_file_sink(const std::filesystem::path& log_file, std::size_t max_size = 1024*1024*4,
                                             std::size_t max_files = 128);
};

}
}
