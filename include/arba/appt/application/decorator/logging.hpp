#pragma once

#include <arba/core/program_args.hpp>

#include <spdlog/logger.h>

#include <filesystem>

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
public:
    static constexpr std::string_view default_logger_name = "application";

protected:
    static std::filesystem::path make_log_dirpath(const core::program_args& args);

    static std::string make_logger_name(const core::program_args& args);
    static spdlog::sink_ptr make_console_sink();
    static spdlog::sink_ptr make_file_sink(const std::filesystem::path& log_fpath);
    static void initialize_logger(std::shared_ptr<spdlog::logger>& logger);
    static void destroy_logger(std::shared_ptr<spdlog::logger> logger);
};

} // namespace private_

template <class ApplicationBase, class SelfType = void>
class logging;

template <class ApplicationBase>
class logging<ApplicationBase> : public ApplicationBase
{
private:
    using base_ = ApplicationBase;

public:
    template <typename OtherType>
    using rebind_t = logging<ApplicationBase, OtherType>;

    using base_::base_;
};

template <class ApplicationBase, class SelfType>
class logging : public logging<typename ApplicationBase::template rebind_t<SelfType>>, private_::logging_impl
{
private:
    using base_ = logging<typename ApplicationBase::template rebind_t<SelfType>>;

public:
    explicit logging(const core::program_args& args = core::program_args());
    ~logging();

    inline std::filesystem::path log_dir() const { return log_fpath_.parent_path(); }
    inline const std::filesystem::path& log_path() const { return log_fpath_; }

    inline const std::shared_ptr<spdlog::logger>& logger() const { return logger_; }
    inline std::shared_ptr<spdlog::logger>& logger() { return logger_; }

protected:
    inline std::filesystem::path make_log_dirpath() const;
    inline std::filesystem::path make_log_filename() const;

    std::shared_ptr<spdlog::logger> make_logger() const;
    inline std::string make_logger_name() const;
    inline std::vector<spdlog::sink_ptr> make_sink_list() const;

    inline void destroy_logger(std::shared_ptr<spdlog::logger> app_logger) const;

private:
    std::filesystem::path log_fpath_;
    std::shared_ptr<spdlog::logger> logger_;
};

template <class ApplicationBase, class SelfType>
logging<ApplicationBase, SelfType>::logging(const core::program_args& args)
    : base_(args), log_fpath_(this->self().make_log_dirpath() / this->self().make_log_filename()),
      logger_(this->self().make_logger())
{
}

template <class ApplicationBase, class SelfType>
logging<ApplicationBase, SelfType>::~logging()
{
    if (logger_)
    {
        this->self().destroy_logger(logger_);
        logger_.reset();
    }
}

template <class ApplicationBase, class SelfType>
std::filesystem::path logging<ApplicationBase, SelfType>::make_log_dirpath() const
{
    return logging_impl::make_log_dirpath(this->args());
}

template <class ApplicationBase, class SelfType>
std::filesystem::path logging<ApplicationBase, SelfType>::make_log_filename() const
{
    return this->self().make_logger_name() + ".log";
}

template <class ApplicationBase, class SelfType>
std::shared_ptr<spdlog::logger> logging<ApplicationBase, SelfType>::make_logger() const
{
    std::vector<spdlog::sink_ptr> sink_list = this->self().make_sink_list();
    std::shared_ptr logger =
        std::make_shared<spdlog::logger>(this->self().make_logger_name(), sink_list.begin(), sink_list.end());
    logger->set_level(static_cast<decltype(spdlog::level::info)>(SPDLOG_ACTIVE_LEVEL));
    logging_impl::initialize_logger(logger);
    return logger;
}

template <class ApplicationBase, class SelfType>
std::string logging<ApplicationBase, SelfType>::make_logger_name() const
{
    return logging_impl::make_logger_name(this->args());
}

template <class ApplicationBase, class SelfType>
std::vector<spdlog::sink_ptr> logging<ApplicationBase, SelfType>::make_sink_list() const
{
    std::vector<spdlog::sink_ptr> sink_list;
    if (spdlog::sink_ptr sink_ptr = this->self().make_console_sink(); sink_ptr) [[likely]]
        sink_list.push_back(std::move(sink_ptr));
    if (spdlog::sink_ptr sink_ptr = this->self().make_file_sink(log_fpath_); sink_ptr) [[likely]]
        sink_list.push_back(std::move(sink_ptr));
    return sink_list;
}

template <class ApplicationBase, class SelfType>
void logging<ApplicationBase, SelfType>::destroy_logger(std::shared_ptr<spdlog::logger> app_logger) const
{
    logging_impl::destroy_logger(app_logger);
}

} // namespace adec
} // namespace appt
} // namespace arba
