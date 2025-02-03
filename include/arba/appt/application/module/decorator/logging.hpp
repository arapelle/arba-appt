#pragma once

#include <arba/appt/application/program_args.hpp>
#include <spdlog/spdlog.h>
#include <source_location>

inline namespace arba
{
namespace appt
{
inline namespace mdec // module_decorator
{
namespace private_
{

class logging_impl
{
protected:
    static spdlog::sink_ptr make_console_sink();
    static spdlog::sink_ptr make_file_sink(const std::filesystem::path& log_fpath);
    static void initialize_logger(std::shared_ptr<spdlog::logger>& logger);
    static void destroy_logger(std::shared_ptr<spdlog::logger> logger);
};

}

template <class ModuleBase, class SelfType = void>
class logging;

template <class ModuleBase>
class logging<ModuleBase> : public ModuleBase
{
private:
    using base_ = ModuleBase;

public:
    using application_type = typename ModuleBase::application_type;

    template <typename OtherType>
    using rebind_t = logging<ModuleBase, OtherType>;

    using base_::base_;
    virtual ~logging() override = default;
};

template <class ModuleBase, class SelfType>
class logging : public logging<typename ModuleBase::template rebind_t<SelfType>>,
                private private_::logging_impl
{
private:
    using base_ = logging<typename ModuleBase::template rebind_t<SelfType>>;

public:
    using application_type = typename base_::application_type;

public:
    explicit logging(application_type& app, std::string_view name = std::string_view())
        : base_(app, name),
        log_fpath_(this->self().make_log_dirpath() / this->self().make_log_filename()),
        logger_(this->self().make_logger())
    {}

    virtual ~logging() override
    {
        if (logger_)
        {
            this->self().destroy_logger(logger_);
            logger_.reset();
        }
    }

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

    inline void destroy_logger(std::shared_ptr<spdlog::logger> mod_logger) const;

    virtual void handle_caught_exception(const std::source_location& location, std::exception_ptr ex_ptr) override;

private:
    std::filesystem::path log_fpath_;
    std::shared_ptr<spdlog::logger> logger_;
};

template <class ModuleBase, class SelfType>
std::filesystem::path logging<ModuleBase, SelfType>::make_log_dirpath() const
{
    return this->app().log_dir();
}

template <class ModuleBase, class SelfType>
std::filesystem::path logging<ModuleBase, SelfType>::make_log_filename() const
{
    return this->self().make_logger_name() + ".log";
}

template <class ModuleBase, class SelfType>
std::shared_ptr<spdlog::logger> logging<ModuleBase, SelfType>::make_logger() const
{
    std::vector<spdlog::sink_ptr> sink_list = this->self().make_sink_list();
    std::shared_ptr logger = std::make_shared<spdlog::logger>(this->self().make_logger_name(),
                                                              sink_list.begin(), sink_list.end());
    logger->set_level(static_cast<decltype(spdlog::level::info)>(SPDLOG_ACTIVE_LEVEL));
    logging_impl::initialize_logger(logger);
    return logger;
}

template <class ModuleBase, class SelfType>
std::string logging<ModuleBase, SelfType>::make_logger_name() const
{
    return this->name();
}

template <class ModuleBase, class SelfType>
std::vector<spdlog::sink_ptr> logging<ModuleBase, SelfType>::make_sink_list() const
{
    std::vector<spdlog::sink_ptr> sink_list;
    if (spdlog::sink_ptr sink_ptr = this->self().make_console_sink(); sink_ptr) [[likely]]
        sink_list.push_back(std::move(sink_ptr));
    if (spdlog::sink_ptr sink_ptr = this->self().make_file_sink(log_fpath_); sink_ptr) [[likely]]
        sink_list.push_back(std::move(sink_ptr));
    return sink_list;
}

template <class ModuleBase, class SelfType>
void logging<ModuleBase, SelfType>::handle_caught_exception(const std::source_location& location,
                                                                     std::exception_ptr ex_ptr)
{
#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL
    std::string error_msg;
    try
    {
        if (ex_ptr)
            std::rethrow_exception(ex_ptr);
    }
    catch (const std::exception& ex)
    {
        error_msg = std::format("Exception caught:\n{}", std::string_view(ex.what()));
    }
    catch (...)
    {
        error_msg = "Unknown exception caught.";
    }

    spdlog::source_loc src_loc(location.file_name(), location.line(), location.function_name());
    (*this->logger()).log(src_loc, spdlog::level::critical, error_msg);
#endif
}

template <class ModuleBase, class SelfType>
void logging<ModuleBase, SelfType>::destroy_logger(std::shared_ptr<spdlog::logger> mod_logger) const
{
    logging_impl::destroy_logger(mod_logger);
}

}
}
}
