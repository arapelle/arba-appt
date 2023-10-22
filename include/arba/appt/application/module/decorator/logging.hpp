#pragma once

#include <arba/appt/application/module/module_logger.hpp>
#include <arba/appt/application/program_args.hpp>
#include <spdlog/spdlog.h>
#include <source_location>

inline namespace arba
{
namespace appt
{
inline namespace mdec // module_decorator
{

template <class module_logger_type, class module_base_type, class module_type = void>
class logging;

template <class module_logger_type, class module_base_type>
class logging<module_logger_type, module_base_type> : public module_base_type
{
private:
    using base_ = module_base_type;

public:
    using application_type = typename module_base_type::application_type;

    template <typename other_module_type>
    using rebind_t = logging<module_logger_type, module_base_type, other_module_type>;

    explicit logging(std::string_view name = std::string_view()) : base_(name) {}
    virtual ~logging() override
    {
        if (logger_)
        {
            spdlog::drop(logger_->name());
            logger_.reset();
        }
    }

    inline const std::shared_ptr<spdlog::logger>& logger() const { return logger_; }
    inline std::shared_ptr<spdlog::logger>& logger() { return logger_; }
    void set_app(application_type& app)
    {
        if (logger_)
        {
            spdlog::drop(logger_->name());
            logger_.reset();
        }
        this->base_::set_app(app);
        logger_ = std::make_shared<module_logger_type>(*this);
        spdlog::register_logger(logger_);
    }

private:
    std::shared_ptr<spdlog::logger> logger_;
};

template <class module_logger_type, class module_base_type, class module_type>
class logging : public logging<module_logger_type, typename module_base_type::template rebind_t<module_type>>
{
private:
    using base_ = logging<module_logger_type, typename module_base_type::template rebind_t<module_type>>;

public:
    using application_type = typename base_::application_type;

public:
    using base_::base_;
    virtual ~logging() override = default;

protected:
    virtual void handle_caught_exception(const std::source_location& location, std::exception_ptr ex_ptr) override;
};

template <class module_logger_type, class module_base_type, class module_type>
void logging<module_logger_type, module_base_type, module_type>::
    handle_caught_exception(const std::source_location& location, std::exception_ptr ex_ptr)
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

}
}
}
