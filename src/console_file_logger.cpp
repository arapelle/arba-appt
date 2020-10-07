#include <appt/util/console_file_logger.hpp>
#include <appt/util/logger_helper.hpp>

inline namespace arba
{
namespace appt
{

console_file_logger::console_file_logger(const std::string &name)
    : console_file_logger(name, logger_helper::log_dir(name)/(name + ".log"))
{}

console_file_logger::console_file_logger(std::string name, const std::filesystem::path &log_file)
    : spdlog::logger(std::move(name),
                     { logger_helper::create_file_sink(log_file), logger_helper::create_console_sink() })
{}

}
}
