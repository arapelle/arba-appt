#include <arba/appt/application/module/decorator/logging.hpp>
#include <arba/appt/util/logging/logger_make_helper.hpp>
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
    return logger_make_helper::make_console_sink_mt();
}

spdlog::sink_ptr logging_impl::make_file_sink(const std::filesystem::path& log_fpath)
{
    return logger_make_helper::make_rotating_file_sink_mt(log_fpath);
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
