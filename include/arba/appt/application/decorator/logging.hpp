#pragma once

#include <arba/appt/application/application_logger.hpp>
#include <arba/appt/application/program_args.hpp>
#include <arba/appt/util/logger_helper.hpp>
#include <spdlog/spdlog.h>

inline namespace arba
{
namespace appt
{
inline namespace adec // application_decorator
{

template <class application_logger_type, class application_base_type, class application_type = void>
class logging;

template <class application_logger_type, class application_base_type>
class logging<application_logger_type, application_base_type> : public application_base_type
{
private:
    using base_ = application_base_type;

public:
    template <typename other_application_type>
    using rebind_t = logging<application_logger_type, application_base_type, other_application_type>;

    explicit logging(const appt::program_args& args = appt::program_args())
        : base_(args),
        log_dir_(logger_helper::log_dir(args.program_stem().empty() ?
                                        std::filesystem::path("application")
                                        : args.program_stem())),
          logger_(std::make_shared<application_logger_type>(*this))
    {
        spdlog::register_logger(logger_);
    }

    inline const std::filesystem::path& log_dir() const { return log_dir_; }
    inline void set_log_dir(const std::filesystem::path& log_dir)
    {
        if (logger_)
        {
            spdlog::drop(logger_->name());
            logger_.reset();
        }
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

template <class application_logger_type, class application_base_type, class application_type>
class logging : public logging<application_logger_type, typename application_base_type::template rebind_t<application_type>>
{
private:
    using base_ = logging<application_logger_type, typename application_base_type::template rebind_t<application_type>>;

public:
    using base_::base_;
};

}
}
}
