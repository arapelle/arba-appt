#pragma once

#include <arba/appt/application/execution_status.hpp>
#include <arba/appt/application/module/basic_module.hpp>
#include <arba/appt/application/module/concepts/concrete_derived_basic_module.hpp>
#include <arba/appt/util/log_critical_message.hpp>

#include <arba/core/sbrm/sbrm.hpp>

#include <format>
#include <memory>
#include <source_location>
#include <thread>

inline namespace arba
{
namespace appt
{
inline namespace adec // application_decorator
{

template <typename ApplicationBase, typename SelfType = void>
class multi_task;

template <typename ApplicationBase>
class multi_task<ApplicationBase> : public ApplicationBase
{
public:
    template <typename OtherType>
    using rebind_t = multi_task<ApplicationBase, OtherType>;

    using ApplicationBase::ApplicationBase;
};

template <typename ApplicationBase, typename SelfType>
class multi_task : public multi_task<typename ApplicationBase::template rebind_t<SelfType>>
{
    using base_ = multi_task<typename ApplicationBase::template rebind_t<SelfType>>;

public:
    using module_base = basic_module<SelfType>;
    using module_base_uptr = std::unique_ptr<module_base>;

    using base_::base_;

    execution_status init();
    [[nodiscard]] execution_status run();
    void stop();
    void stop_side_modules();

    [[nodiscard]] inline execution_status init_status() const noexcept { return init_status_; }
    [[nodiscard]] inline execution_status run_status() const noexcept { return run_status_; }

    void set_main_module(module_base_uptr module_uptr);

    void add_module(module_base_uptr module_uptr);

    template <ConcreteDerivedBasicModule module_type>
    module_type& set_main_module(std::unique_ptr<module_type> module_uptr);

    template <ConcreteDerivedBasicModule module_type>
    module_type& add_module(std::unique_ptr<module_type> module_uptr);

    template <ConcreteDerivedBasicModule module_type>
    module_type& create_module(std::string_view module_name);

    template <ConcreteDerivedBasicModule module_type>
    module_type& create_module();

    template <ConcreteDerivedBasicModule module_type>
    module_type& create_main_module(std::string_view module_name);

    template <ConcreteDerivedBasicModule module_type>
    module_type& create_main_module();

    void log_critical_message(const std::source_location& location, std::string_view message);

protected:
    inline void handle_caught_exception(const std::source_location location, std::exception_ptr ex_ptr);

private:
    bool check_init_status_();
    void join_side_modules_();

private:
    std::vector<std::pair<module_base_uptr, std::thread>> side_modules_;
    module_base_uptr main_module_;
    execution_status init_status_ = execution_statuses::ready;
    execution_status run_status_ = execution_statuses::ready;
    std::mutex mutex_;
};

template <typename ApplicationBase, typename SelfType>
execution_status multi_task<ApplicationBase, SelfType>::init()
{
    try
    {
        init_status_ = execution_statuses::executing;
        if (main_module_)
        {
            if (main_module_->init(meta::maythrow) != execution_statuses::success)
                init_status_ = execution_statuses::failure;
        }
        if (init_status_ == execution_statuses::executing) [[likely]]
        {
            for (auto& entry : side_modules_)
            {
                init_status_ = entry.first->init(meta::maythrow);
                if (init_status_ != execution_statuses::success) [[unlikely]]
                {
                    init_status_ = execution_statuses::failure;
                    break;
                }
            }
        }
        init_status_ = execution_statuses::success;
    }
    catch (...)
    {
        init_status_ = execution_statuses::failure;
        this->self().handle_caught_exception(std::source_location::current(), std::current_exception());
    }
    return init_status_;
}

template <typename ApplicationBase, typename SelfType>
execution_status multi_task<ApplicationBase, SelfType>::run()
{
    try
    {
        if (check_init_status_() == false) [[unlikely]]
            return run_status_;

        run_status_ = execution_statuses::executing;

        core::sbrm join_side_modules = [this] { join_side_modules_(); };
        core::sbrm stop_side_modules_iferr = [this] { stop_side_modules(); };

        for (auto& entry : side_modules_)
        {
            module_base* module_ptr = entry.first.get();
            using run_callback_t = void (module_base::*)(std::nothrow_t);
            entry.second = std::thread(static_cast<run_callback_t>(&module_base::run), module_ptr, std::nothrow);
        }
        if (main_module_)
            main_module_->run(meta::maythrow);

        stop_side_modules_iferr.disable();
    }
    catch (...)
    {
        run_status_ = execution_statuses::failure;
        this->self().handle_caught_exception(std::source_location::current(), std::current_exception());
    }

    if (run_status_ == execution_statuses::executing)
        run_status_ = execution_statuses::success;
    return run_status_;
}

template <typename ApplicationBase, typename SelfType>
bool multi_task<ApplicationBase, SelfType>::check_init_status_()
{
    if (init_status_ != execution_statuses::success) [[unlikely]]
    {
        if (init_status_ == execution_statuses::ready)
            throw std::runtime_error("init status is 'ready'. Did you forget to call init()?");
        run_status_ = execution_statuses::failure;
        return false;
    }
    return true;
}

template <typename ApplicationBase, typename SelfType>
void multi_task<ApplicationBase, SelfType>::join_side_modules_()
{
    for (auto& entry : side_modules_)
    {
        if (entry.second.joinable())
            entry.second.join();
        if (entry.first->run_status() != execution_statuses::success)
            run_status_ = execution_statuses::failure;
    }
}

template <typename ApplicationBase, typename SelfType>
void multi_task<ApplicationBase, SelfType>::handle_caught_exception(const std::source_location location,
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

    this->self().log_critical_message(location, error_msg);
}

template <typename ApplicationBase, typename SelfType>
void multi_task<ApplicationBase, SelfType>::log_critical_message(const std::source_location& location,
                                                                 std::string_view message)
{
    if constexpr (requires(base_& app) {
                      { app.log_critical_message(location, message) };
                  })
    {
        this->base_::log_critical_message(location, message);
    }
    else
    {
        log_critical_message_to_cerr(message);
    }
}

template <typename ApplicationBase, typename SelfType>
void multi_task<ApplicationBase, SelfType>::stop_side_modules()
{
    std::lock_guard lock(mutex_);
    for (auto& entry : side_modules_)
        entry.first->stop();
}

template <typename ApplicationBase, typename SelfType>
void multi_task<ApplicationBase, SelfType>::stop()
{
    stop_side_modules();
    std::lock_guard lock(mutex_);
    if (main_module_)
        main_module_->stop();
}

template <typename ApplicationBase, typename SelfType>
void multi_task<ApplicationBase, SelfType>::set_main_module(module_base_uptr module_uptr)
{
    assert((&module_uptr->app()) == (&this->self()));
    main_module_ = std::move(module_uptr);
}

template <typename ApplicationBase, typename SelfType>
template <ConcreteDerivedBasicModule module_type>
module_type& multi_task<ApplicationBase, SelfType>::set_main_module(std::unique_ptr<module_type> module_uptr)
{
    assert((&module_uptr->app()) == (&this->self()));
    module_type* module_ptr = module_uptr.get();
    main_module_ = std::move(module_uptr);
    return *module_ptr;
}

template <typename ApplicationBase, typename SelfType>
void multi_task<ApplicationBase, SelfType>::add_module(module_base_uptr module_uptr)
{
    assert((&module_uptr->app()) == (&this->self()));
    side_modules_.emplace_back(std::move(module_uptr), std::thread());
}

template <typename ApplicationBase, typename SelfType>
template <ConcreteDerivedBasicModule module_type>
module_type& multi_task<ApplicationBase, SelfType>::add_module(std::unique_ptr<module_type> module_uptr)
{
    assert((&module_uptr->app()) == (&this->self()));
    module_type* module_ptr = module_uptr.get();
    side_modules_.emplace_back(std::move(module_uptr), std::thread());
    return *module_ptr;
}

template <typename ApplicationBase, typename SelfType>
template <ConcreteDerivedBasicModule module_type>
module_type& multi_task<ApplicationBase, SelfType>::create_module(std::string_view module_name)
{
    std::unique_ptr module_uptr = std::make_unique<module_type>(this->self(), module_name);
    return add_module<module_type>(std::move(module_uptr));
}

template <typename ApplicationBase, typename SelfType>
template <ConcreteDerivedBasicModule module_type>
module_type& multi_task<ApplicationBase, SelfType>::create_main_module(std::string_view module_name)
{
    std::unique_ptr module_uptr = std::make_unique<module_type>(this->self(), module_name);
    return set_main_module<module_type>(std::move(module_uptr));
}

template <typename ApplicationBase, typename SelfType>
template <ConcreteDerivedBasicModule module_type>
module_type& multi_task<ApplicationBase, SelfType>::create_module()
{
    std::unique_ptr module_uptr = std::make_unique<module_type>(this->self());
    return add_module<module_type>(std::move(module_uptr));
}

template <typename ApplicationBase, typename SelfType>
template <ConcreteDerivedBasicModule module_type>
module_type& multi_task<ApplicationBase, SelfType>::create_main_module()
{
    std::unique_ptr module_uptr = std::make_unique<module_type>(this->self());
    return set_main_module<module_type>(std::move(module_uptr));
}

} // namespace adec
} // namespace appt
} // namespace arba
