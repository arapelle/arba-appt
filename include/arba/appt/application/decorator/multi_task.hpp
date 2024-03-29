#pragma once

#include <source_location>
#include <memory>
#include <format>
#include <thread>
#include <spdlog/spdlog.h>
#include <arba/core/sbrm.hpp>
#include <arba/appt/application/execution_status.hpp>
#include <arba/appt/application/module/module_base.hpp>
#include <arba/appt/application/module/basic_module.hpp>
#include <arba/appt/util/logging/log_critical_message.hpp>

inline namespace arba
{
namespace appt
{
inline namespace adec // application_decorator
{

template <typename application_base_type, typename application_type = void>
class multi_task;

template <typename application_base_type>
class multi_task<application_base_type> : public application_base_type
{
public:
    template <typename other_application_type>
    using rebind_t = multi_task<application_base_type, other_application_type>;

    using application_base_type::application_base_type;
};


template <typename application_base_type, typename application_type>
class multi_task : public multi_task<typename application_base_type::template rebind_t<application_type>>
{
    using base_ = multi_task<typename application_base_type::template rebind_t<application_type>>;
    using module_base_ = basic_module<application_type>;
    using module_base_uptr_ = std::unique_ptr<module_base_>;

public:
    using base_::base_;

    execution_status init();
    [[nodiscard]] execution_status run();
    void stop();
    void stop_side_modules();

    [[nodiscard]] inline execution_status init_status() const noexcept { return init_status_; }
    [[nodiscard]] inline execution_status run_status() const noexcept { return run_status_; }

    template <typename module_type>
        requires std::is_base_of_v<module_base_, module_type> && (!std::is_abstract_v<module_type>)
    module_type& set_main_module(std::unique_ptr<module_type> module_uptr);

    template <typename module_type>
        requires std::is_base_of_v<module_base_, module_type> && (!std::is_abstract_v<module_type>)
    module_type& add_module(std::unique_ptr<module_type> module_uptr);

    template <typename module_type>
        requires std::is_base_of_v<module_base_, module_type> && (!std::is_abstract_v<module_type>)
    module_type& create_module(std::string_view module_name);

    template <typename module_type>
        requires std::is_base_of_v<module_base_, module_type> && (!std::is_abstract_v<module_type>)
    module_type& create_module();

    template <typename module_type>
        requires std::is_base_of_v<module_base_, module_type> && (!std::is_abstract_v<module_type>)
    module_type& create_main_module(std::string_view module_name);

    template <typename module_type>
        requires std::is_base_of_v<module_base_, module_type> && (!std::is_abstract_v<module_type>)
    module_type& create_main_module();

protected:
    inline void handle_caught_exception(const std::source_location location, std::exception_ptr ex_ptr);

private:
    bool check_init_status_();
    void join_side_modules_();

private:
    std::vector<std::pair<module_base_uptr_, std::thread>> side_modules_;
    module_base_uptr_ main_module_;
    execution_status init_status_ = execution_status::ready;
    execution_status run_status_ = execution_status::ready;
    std::mutex mutex_;
};

template <typename application_base_type, typename application_type>
execution_status multi_task<application_base_type, application_type>::init()
{
    try
    {
        init_status_ = execution_status::executing;
        if (main_module_)
        {
            if (main_module_->init(core::maythrow) != execution_status::execution_success)
                init_status_ = execution_status::execution_failure;
        }
        if (init_status_ == execution_status::executing) [[likely]]
        {
            for (auto& entry : side_modules_)
            {
                init_status_ = entry.first->init(core::maythrow);
                if (init_status_ != execution_status::execution_success) [[unlikely]]
                {
                    init_status_ = execution_status::execution_failure;
                    break;
                }
            }
        }
        init_status_ = execution_status::execution_success;
    }
    catch (...)
    {
        init_status_ = execution_status::execution_failure;
        this->self().handle_caught_exception(std::source_location::current(), std::current_exception());
    }
    return init_status_;
}

template <typename application_base_type, typename application_type>
execution_status multi_task<application_base_type, application_type>::run()
{
    try
    {
        if (check_init_status_() == false) [[unlikely]]
            return run_status_;

        run_status_ = execution_status::executing;

        core::sbrm join_side_modules = [this]{ join_side_modules_(); };
        core::sbrm stop_side_modules_iferr = [this]{ stop_side_modules(); };

        for (auto& entry : side_modules_)
        {
            module_base_* module_ptr = entry.first.get();
            using run_callback_t = void (module_base_::*)(std::nothrow_t);
            entry.second = std::thread(static_cast<run_callback_t>(&module_base_::run), module_ptr, std::nothrow);
        }
        if (main_module_)
            main_module_->run(core::maythrow);

        stop_side_modules_iferr.disable();
    }
    catch (...)
    {
        run_status_ = execution_status::execution_failure;
        this->self().handle_caught_exception(std::source_location::current(), std::current_exception());
    }

    if (run_status_ == execution_status::executing)
        run_status_ = execution_status::execution_success;
    return run_status_;
}

template <typename application_base_type, typename application_type>
bool multi_task<application_base_type, application_type>::check_init_status_()
{
    if (init_status_ != execution_status::execution_success) [[unlikely]]
    {
        if (init_status_ == execution_status::ready)
            throw std::runtime_error("init status is 'ready'. Did you forget to call init()?");
        run_status_ = execution_status::execution_failure;
        return false;
    }
    return true;
}

template <typename application_base_type, typename application_type>
void multi_task<application_base_type, application_type>::join_side_modules_()
{
    for (auto& entry : side_modules_)
    {
        if (entry.second.joinable())
            entry.second.join();
        if (entry.first->run_status() != execution_status::execution_success)
            run_status_ = execution_status::execution_failure;
    }
}


template <typename application_base_type, typename application_type>
void multi_task<application_base_type, application_type>::handle_caught_exception(const std::source_location location,
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
        (*this->self().logger()).log(src_loc, spdlog::level::critical, error_msg);
#endif
    }
    else
    {
        log_critical_message_to_cerr(error_msg);
    }
}

template <typename application_base_type, typename application_type>
void multi_task<application_base_type, application_type>::stop_side_modules()
{
    std::lock_guard lock(mutex_);
    for (auto& entry : side_modules_)
        entry.first->stop();
}

template <typename application_base_type, typename application_type>
void multi_task<application_base_type, application_type>::stop()
{
    stop_side_modules();
    std::lock_guard lock(mutex_);
    if (main_module_)
        main_module_->stop();
}

template <typename application_base_type, typename application_type>
template <typename module_type>
    requires std::is_base_of_v<typename multi_task<application_base_type, application_type>::module_base_, module_type>
             && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::set_main_module(std::unique_ptr<module_type> module_uptr)
{
    module_uptr->set_app(this->self());
    module_type* module_ptr = module_uptr.get();
    main_module_ = std::move(module_uptr);
    return *module_ptr;
}

template <typename application_base_type, typename application_type>
template <typename module_type>
    requires std::is_base_of_v<typename multi_task<application_base_type, application_type>::module_base_, module_type>
             && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::add_module(std::unique_ptr<module_type> module_uptr)
{
    module_uptr->set_app(this->self());
    module_type* module_ptr = module_uptr.get();
    side_modules_.emplace_back(std::move(module_uptr), std::thread());
    return *module_ptr;
}

template <typename application_base_type, typename application_type>
template <typename module_type>
    requires std::is_base_of_v<typename multi_task<application_base_type, application_type>::module_base_, module_type>
             && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::create_module(std::string_view module_name)
{
    std::unique_ptr module_uptr = std::make_unique<module_type>(module_name);
    return add_module<module_type>(std::move(module_uptr));
}

template <typename application_base_type, typename application_type>
template <typename module_type>
    requires std::is_base_of_v<typename multi_task<application_base_type, application_type>::module_base_, module_type>
             && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::create_main_module(std::string_view module_name)
{
    std::unique_ptr module_uptr = std::make_unique<module_type>(module_name);
    return set_main_module<module_type>(std::move(module_uptr));
}

template <typename application_base_type, typename application_type>
template <typename module_type>
    requires std::is_base_of_v<typename multi_task<application_base_type, application_type>::module_base_, module_type>
             && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::create_module()
{
    std::unique_ptr module_uptr = std::make_unique<module_type>();
    return add_module<module_type>(std::move(module_uptr));
}

template <typename application_base_type, typename application_type>
template <typename module_type>
    requires std::is_base_of_v<typename multi_task<application_base_type, application_type>::module_base_, module_type>
             && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::create_main_module()
{
    std::unique_ptr module_uptr = std::make_unique<module_type>();
    return set_main_module<module_type>(std::move(module_uptr));
}

}
}
}
