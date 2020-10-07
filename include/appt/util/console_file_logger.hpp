#pragma once

#include <spdlog/logger.h>
#include <filesystem>

inline namespace arba
{
namespace appt
{

class console_file_logger : public spdlog::logger
{
public:
    explicit console_file_logger(const std::string& name);
    console_file_logger(std::string name, const std::filesystem::path& log_file);
};

}
}
