#include <arba/appt/application/decorator/logging.hpp>
#include <arba/appt/util/format_time_point.hpp>
#include <arba/appt/util/logging/logger_make_helper.hpp>

#include <spdlog/spdlog.h>

inline namespace arba
{
namespace appt
{
inline namespace adec // application_decorator
{
namespace private_
{

std::filesystem::path logging_impl::make_log_dirpath(const core::program_args& args)
{
    return std::filesystem::temp_directory_path() / make_logger_name(args) / "log" / format_filename_Ymd_HMS_mcs();
}

std::string logging_impl::make_logger_name(const core::program_args& args)
{
    return args.empty() ? std::string(default_logger_name) : args.program_stem().generic_string();
}

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
    spdlog::set_default_logger(logger);
}

void logging_impl::destroy_logger(std::shared_ptr<spdlog::logger> logger)
{
    spdlog::drop(logger->name());
}

} // namespace private_
} // namespace adec
} // namespace appt
} // namespace arba
