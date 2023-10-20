#pragma once

#include <arba/core/sbrm.hpp>
#include <arba/core/debug/assert.hpp>
#include <arba/core/template/exception_policy.hpp>
#include <arba/appt/application/execution_status.hpp>
#include <arba/appt/util/log_critical_message.hpp>
#include <spdlog/spdlog.h>
#include <source_location>
#include <exception>
#include "module_base.hpp"

inline namespace arba
{
namespace appt
{

template <class ApplicationType, class ModuleType = void>
class basic_module;

template <class ApplicationType>
class basic_module<ApplicationType, void> : public module_base
{
public:
    template <typename OtherModuleType>
    using rebind_t = basic_module<ApplicationType, OtherModuleType>;

    using application_type = ApplicationType;

    explicit basic_module(std::string_view name = std::string_view()) : module_base(name) {}
    virtual ~basic_module() override = default;

    inline const application_type& app() const { return *application_; }
    inline application_type& app() { return *application_; }
    void set_app(application_type& app);

    virtual void init();
    void run(core::maythrow_t);
    void run(std::nothrow_t);
    virtual void stop() {}

    [[nodiscard]] inline execution_status run_status() const noexcept { return run_status_; }

protected:
    virtual void run() = 0;
    virtual void handle_caught_exception(const std::source_location& location, std::exception_ptr ex_ptr);

private:
    application_type* application_ = nullptr;
    execution_status run_status_ = execution_status::ready;
};

// Template methods implementation:

template <class ApplicationType>
void basic_module<ApplicationType>::set_app(application_type& app)
{
    if (application_ && application_ != &app) [[unlikely]]
        throw std::runtime_error("Module is already linked to an application.");
    application_ = &app;
}

template <class ApplicationType>
void basic_module<ApplicationType>::init()
{
    ARBA_ASSERT(application_ != nullptr);
}

template <class ApplicationType>
void basic_module<ApplicationType>::run(core::maythrow_t)
{
    run_status_ = execution_status::executing;
    core::sbrm set_execution_failure_if_err = [this]{ run_status_ = execution_status::execution_failure; };
    this->run();
    set_execution_failure_if_err.disable();
    run_status_ = execution_status::execution_success;
}

template <class ApplicationType>
void basic_module<ApplicationType>::run(std::nothrow_t)
{
    try
    {
        run(core::maythrow);
    }
    catch (...)
    {
        this->handle_caught_exception(std::source_location::current(), std::current_exception());
        if constexpr (requires(application_type& app){ { app.stop() }; })
        {
            app().stop();
        }
    }
}

template <class ApplicationType>
void basic_module<ApplicationType>::handle_caught_exception(const std::source_location& location,
                                                            std::exception_ptr ex_ptr)
{
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

    if constexpr (requires(application_type& app){ { *(app.logger()) } -> std::convertible_to<spdlog::logger&>; })
    {
#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL
        spdlog::source_loc src_loc(location.file_name(), location.line(), location.function_name());
        (*this->app().logger()).log(src_loc, spdlog::level::critical, error_msg);
#endif
    }
    else
    {
        log_critical_message_to_cerr(error_msg);
    }
}


template <class ApplicationType, class ModuleType>
class basic_module : public basic_module<ApplicationType>
{
private:
    using base_ = basic_module<ApplicationType>;

public:
    using base_::base_;

protected:
    using self_type_ = ModuleType;

    [[nodiscard]] inline const self_type_& self_() const noexcept { return static_cast<self_type_&>(*this); }
    [[nodiscard]] inline self_type_& self_() noexcept { return static_cast<self_type_&>(*this); }
};

}
}
