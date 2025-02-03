#pragma once

#include <arba/core/sbrm/sbrm.hpp>
#include <arba/meta/policy/exception_policy.hpp>
#include <arba/appt/application/execution_status.hpp>
#include <arba/appt/util/logging/log_critical_message.hpp>
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
    template <typename OtherType>
    using rebind_t = basic_module<ApplicationType, OtherType>;

    using application_type = ApplicationType;

    explicit basic_module(application_type& app, std::string_view name = std::string_view())
        : module_base(name), application_(&app)
    {}
    virtual ~basic_module() override = default;

    inline const application_type& app() const { return *application_; }
    inline application_type& app() { return *application_; }
    void set_app(application_type& app);

    execution_status init(meta::maythrow_t)
    {
        core::sbrm set_execution_failure_if_err = [this]{ init_status_ = execution_status::execution_failure; };
        this->init();
        if (init_status_ == execution_status::ready) [[unlikely]]
            throw std::runtime_error("The init status is 'ready' (should be at least 'executing'). "
                                     "Did you forget to call parent init()?");
        set_execution_failure_if_err.disable();
        if (init_status_ == execution_status::executing) [[likely]]
            init_status_ = execution_status::execution_success;
        return init_status_;
    }

    void run(meta::maythrow_t);
    void run(std::nothrow_t);
    virtual void stop() {}

    [[nodiscard]] inline execution_status init_status() const noexcept { return init_status_; }
    [[nodiscard]] inline execution_status run_status() const noexcept { return run_status_; }

protected:
    virtual void init();
    virtual void run() = 0;
    virtual void handle_caught_exception(const std::source_location& location, std::exception_ptr ex_ptr);

private:
    application_type* application_ = nullptr;
    execution_status init_status_ = execution_status::ready;
    execution_status run_status_ = execution_status::ready;

    template <class /*ApplicationType*/, class /*ModuleType*/>
    friend class basic_module;
};

// Template methods implementation:

template <class ApplicationType>
void basic_module<ApplicationType>::init()
{
    throw std::runtime_error("CRTP class is not used correctly.\n"
                             "You forgot to provide the ModuleType in your custom module type,\n"
                             "or you are using a decorator around basic_module<AppType> "
                             "which forgot to call rebind_t<OtherType>,\n"
                             "or you are doing something very nasty. >.>");
}

template <class ApplicationType>
void basic_module<ApplicationType>::run(meta::maythrow_t)
{
    assert(init_status_ == execution_status::execution_success);
    run_status_ = execution_status::executing;
    core::sbrm set_execution_failure_if_err = [this]{ run_status_ = execution_status::execution_failure; };
    this->run();
    set_execution_failure_if_err.disable();
    if (run_status_ == execution_status::executing) [[likely]]
        run_status_ = execution_status::execution_success;
}

template <class ApplicationType>
void basic_module<ApplicationType>::run(std::nothrow_t)
{
    try
    {
        run(meta::maythrow);
    }
    catch (...)
    {
        this->handle_caught_exception(std::source_location::current(), std::current_exception());
        if constexpr (requires(application_type& app){ { app.stop() }; })
        {
            app().stop();
        }
        else
        {
            throw;
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
    using self_type = ModuleType;

    using base_::base_;

protected:
    virtual void init()
    {
        this->init_status_ = execution_status::executing;
        assert(&(this->app()) != nullptr);
    }

protected:
    [[nodiscard]] inline const self_type& self() const noexcept { return static_cast<const self_type&>(*this); }
    [[nodiscard]] inline self_type& self() noexcept { return static_cast<self_type&>(*this); }
};

}
}
