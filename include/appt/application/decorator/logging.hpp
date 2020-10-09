#pragma once

#include <appt/application/application_logger.hpp>
#include <appt/application/program_args.hpp>
#include <appt/util/logger_helper.hpp>
#include <spdlog/spdlog.h>

inline namespace arba
{
namespace appt::adec
{

template <class application_logger_type, class application_base_type>
class logging : public application_base_type
{
private:
    using base_ = application_base_type;

public:
    logging(int argc, char** argv) : logging(appt::program_args(argc, argv)) {}
    explicit logging(const appt::program_args& args = appt::program_args())
        : base_(args),
          log_dir_(logger_helper::log_dir(args.program_stem().empty() ? "application" : args.program_stem())),
          logger_(std::make_shared<application_logger_type>(*this))
    {
        spdlog::register_logger(logger_);
    }

    inline const std::filesystem::path& log_dir() const { return log_dir_; }
    inline void set_log_dir(const std::filesystem::path& log_dir)
    {
        log_dir_ = log_dir;
        logger_ = std::make_shared<application_logger_type>(*this);
        spdlog::register_logger(logger_);
    }
    inline const std::shared_ptr<spdlog::logger>& logger() const { return logger_; }
    inline std::shared_ptr<spdlog::logger>& logger() { return logger_; }

private:
    std::filesystem::path log_dir_;
    std::shared_ptr<spdlog::logger> logger_;
};

}
}
