#include <arba/appt/util/console_file_logger.hpp>
#include <arba/appt/util/logger_helper.hpp>
#include <spdlog/spdlog.h>

inline namespace arba
{
namespace appt
{

console_file_logger::console_file_logger(const std::string &name)
    : console_file_logger(name, logger_helper::log_dir(name)/(name + ".log"))
{
    set_level(spdlog::get_level());
}

console_file_logger::console_file_logger(std::string name, const std::filesystem::path &log_file)
    : spdlog::logger(std::move(name),
                     { logger_helper::create_file_sink(log_file), logger_helper::create_console_sink() })
{
    set_level(spdlog::get_level());
}

}
}
