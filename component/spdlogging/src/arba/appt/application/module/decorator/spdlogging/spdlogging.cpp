#include <arba/appt/application/module/decorator/spdlogging/spdlogging.hpp>
#include <arba/appt/util/spdlogging/spdlog_helper.hpp>

#include <spdlog/spdlog.h>

inline namespace arba
{
namespace appt
{
inline namespace mdec // module_decorator
{
namespace private_
{

spdlog::sink_ptr spdlogging_impl::make_console_sink()
{
    return spdlog_helper::make_console_sink_mt();
}

spdlog::sink_ptr spdlogging_impl::make_file_sink(const std::filesystem::path& log_fpath)
{
    return spdlog_helper::make_rotating_file_sink_mt(log_fpath);
}

void spdlogging_impl::initialize_logger(std::shared_ptr<spdlog::logger>& logger)
{
    spdlog::register_logger(logger);
}

void spdlogging_impl::destroy_logger(std::shared_ptr<spdlog::logger> logger)
{
    spdlog::drop(logger->name());
}

} // namespace private_
} // namespace mdec
} // namespace appt
} // namespace arba
