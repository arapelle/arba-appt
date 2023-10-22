#pragma once

#include <arba/appt/application/program_args.hpp>
#include <filesystem>
#include <spdlog/logger.h>

inline namespace arba
{
namespace appt
{
inline namespace adec // application_decorator
{
namespace private_
{

class logging_impl
{
protected:
    static std::filesystem::path make_log_directory_path(const program_args& args);

    static std::string make_logger_name(const program_args& args);
    static spdlog::sink_ptr make_console_sink();
    static spdlog::sink_ptr make_file_sink(const std::filesystem::path& log_fpath);
    static void initialize_logger(std::shared_ptr<spdlog::logger>& logger);
    static void destroy_logger(std::shared_ptr<spdlog::logger> logger);
};

}

template <class application_base_type, class application_type = void>
class logging;

template <class application_base_type>
class logging<application_base_type> : public application_base_type
{
private:
    using base_ = application_base_type;

public:
    template <typename other_application_type>
    using rebind_t = logging<application_base_type, other_application_type>;

    using base_::base_;
};

template <class application_base_type, class application_type>
class logging : public logging<typename application_base_type::template rebind_t<application_type>>,
                private_::logging_impl
{
private:
    using base_ = logging<typename application_base_type::template rebind_t<application_type>>;

public:
    explicit logging(const appt::program_args& args = appt::program_args())
        : base_(args),
        log_dir_(this->self().make_log_directory_path()),
        logger_(this->self().make_logger())
    {}

    ~logging()
    {
        if (logger_)
        {
            this->self().destroy_logger(logger_);
            logger_.reset();
        }
    }

    inline const std::filesystem::path& log_directory() const { return log_dir_; }

    inline const std::shared_ptr<spdlog::logger>& logger() const { return logger_; }
    inline std::shared_ptr<spdlog::logger>& logger() { return logger_; }

protected:
    inline std::filesystem::path make_log_directory_path() const;

    std::shared_ptr<spdlog::logger> make_logger() const;
    inline std::string make_logger_name() const;
    inline std::filesystem::path make_logger_filename() const;
    inline std::vector<spdlog::sink_ptr> make_sink_list() const;
    inline void destroy_logger(std::shared_ptr<spdlog::logger> app_logger) const;

private:
    std::filesystem::path log_dir_;
    std::shared_ptr<spdlog::logger> logger_;
};

template <class application_base_type, class application_type>
std::string logging<application_base_type, application_type>::make_logger_name() const
{
    return logging_impl::make_logger_name(this->args());
}

template <class application_base_type, class application_type>
std::filesystem::path logging<application_base_type, application_type>::make_logger_filename() const
{
    return this->self().make_logger_name() + ".log";
}

template <class application_base_type, class application_type>
std::filesystem::path logging<application_base_type, application_type>::make_log_directory_path() const
{
    return logging_impl::make_log_directory_path(this->args());
}

template <class application_base_type, class application_type>
std::shared_ptr<spdlog::logger> logging<application_base_type, application_type>::make_logger() const
{
    std::vector<spdlog::sink_ptr> sink_list = this->self().make_sink_list();
    std::shared_ptr logger = std::make_shared<spdlog::logger>(this->self().make_logger_name(),
                                                              sink_list.begin(), sink_list.end());
    logger->set_level(static_cast<decltype(spdlog::level::info)>(SPDLOG_ACTIVE_LEVEL));
    logging_impl::initialize_logger(logger);
    return logger;
}

template <class application_base_type, class application_type>
std::vector<spdlog::sink_ptr> logging<application_base_type, application_type>::make_sink_list() const
{
    std::vector<spdlog::sink_ptr> sink_list;

    if (spdlog::sink_ptr sink_ptr = this->self().make_console_sink(); sink_ptr)
        sink_list.push_back(std::move(sink_ptr));

    std::filesystem::path log_fpath = log_dir_ / this->self().make_logger_filename();
    if (spdlog::sink_ptr sink_ptr = this->self().make_file_sink(log_fpath); sink_ptr)
        sink_list.push_back(std::move(sink_ptr));

    return sink_list;
}

template <class application_base_type, class application_type>
void logging<application_base_type, application_type>::destroy_logger(std::shared_ptr<spdlog::logger> app_logger) const
{
    logging_impl::destroy_logger(app_logger);
}

}
}
}
