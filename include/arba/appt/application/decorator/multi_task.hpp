#pragma once

#include <arba/core/sbrm.hpp>
#include <arba/appt/application/module/module.hpp>
#include <memory>
#include <format>
#include <thread>
#include <spdlog/spdlog.h>

inline namespace arba
{
namespace appt
{
inline namespace adec // application_decorator
{
namespace private_
{

void notify_error_message_to_cerr_(std::string_view error_msg);

}

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

public:
    using base_::base_;

    void init();
    void run();
    void stop_side_modules();

    template <typename module_type>
    requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
    module_type& set_main_module(std::unique_ptr<module_type>&& module_uptr);

    template <typename module_type>
    requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
    module_type& add_module(std::unique_ptr<module_type>&& module_uptr);

    template <typename module_type>
        requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
    module_type& create_module(std::string_view module_name);

    template <typename module_type>
        requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
    module_type& create_module();

    template <typename module_type>
        requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
    module_type& create_main_module(std::string_view module_name);

    template <typename module_type>
        requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
    module_type& create_main_module();

protected:
    inline void on_exception_raised_(std::string_view context, const std::exception& ex);
    inline void on_unknown_exception_raised_(std::string_view context);

    template <class ApplicationType>
        requires requires(ApplicationType app)
    {
        { *(app.logger()) } -> std::convertible_to<spdlog::logger&>;
    }
    static void notify_exception_(ApplicationType& app, std::string_view context, const std::exception* ex = nullptr);

    template <class ApplicationType>
        requires (!requires(ApplicationType app)
    {
        { *(app.logger()) } -> std::convertible_to<spdlog::logger&>;
    })
    static void notify_exception_(ApplicationType&, std::string_view context, const std::exception* ex = nullptr);

private:
    using module_interface_uptr = std::unique_ptr<module_interface>;
    std::vector<std::pair<module_interface_uptr, std::thread>> side_modules_;
    module_interface_uptr main_module_;
    std::mutex mutex_;
};

template <typename application_base_type, typename application_type>
void multi_task<application_base_type, application_type>::init()
{
    try
    {
        if (main_module_)
            main_module_->init();
        for (auto& entry : side_modules_)
            entry.first->init();
    }
    catch (const std::exception& ex)
    {
        this->self_().on_exception_raised_("Exception caught in multi_task::init()", ex);
    }
    catch (...)
    {
        this->self_().on_unknown_exception_raised_("Unknown exception caught in multi_task::init()");
    }
}

template <typename application_base_type, typename application_type>
void multi_task<application_base_type, application_type>::run()
{
    try
    {
        core::sbrm join_side_modules = [this]
        {
            for (auto& entry : side_modules_)
                if (entry.second.joinable())
                    entry.second.join();
        };
        core::sbrm stop_side_modules_iferr = [this]{ stop_side_modules(); };

        for (auto& entry : side_modules_)
        {
            module_interface* module_ptr = entry.first.get();
            entry.second = std::thread(&module_interface::run, module_ptr);
        }

        if (main_module_)
            main_module_->run();

        stop_side_modules_iferr.disable();
    }
    catch (const std::exception& ex)
    {
        this->self_().on_exception_raised_("Exception caught in multi_task<App>::run()", ex);
    }
    catch (...)
    {
        this->self_().on_unknown_exception_raised_("Unknown exception caught in multi_task<App>::run()");
    }
}

template <typename application_base_type, typename application_type>
void multi_task<application_base_type, application_type>::on_exception_raised_(std::string_view context,
                                                                               const std::exception& ex)
{
    notify_exception_(this->self_(), context, &ex);
}

template <typename application_base_type, typename application_type>
void multi_task<application_base_type, application_type>::on_unknown_exception_raised_(std::string_view context)
{
    notify_exception_(this->self_(), context);
}

template <typename application_base_type, typename application_type>
void multi_task<application_base_type, application_type>::stop_side_modules()
{
    std::lock_guard lock(mutex_);
    for (auto& entry : side_modules_)
        entry.first->stop();
}

template <typename application_base_type, typename application_type>
template <typename module_type>
requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::set_main_module(std::unique_ptr<module_type>&& module_uptr)
{
    module_uptr->set_app(this->self_());
    module_type* module_ptr = module_uptr.get();
    main_module_ = std::move(module_uptr);
    return *module_ptr;
}

template <typename application_base_type, typename application_type>
template <typename module_type>
requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::add_module(std::unique_ptr<module_type>&& module_uptr)
{
    module_uptr->set_app(this->self_());
    module_type* module_ptr = module_uptr.get();
    side_modules_.emplace_back(std::move(module_uptr), std::thread());
    return *module_ptr;
}

template <typename application_base_type, typename application_type>
template <typename module_type>
    requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::create_module(std::string_view module_name)
{
    std::unique_ptr module_uptr = std::make_unique<module_type>(module_name);
    return add_module<module_type>(std::move(module_uptr));
}

template <typename application_base_type, typename application_type>
template <typename module_type>
    requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::create_main_module(std::string_view module_name)
{
    std::unique_ptr module_uptr = std::make_unique<module_type>(module_name);
    return set_main_module<module_type>(std::move(module_uptr));
}

template <typename application_base_type, typename application_type>
template <typename module_type>
requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::create_module()
{
    std::unique_ptr module_uptr = std::make_unique<module_type>();
    return add_module<module_type>(std::move(module_uptr));
}

template <typename application_base_type, typename application_type>
template <typename module_type>
requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::create_main_module()
{
    std::unique_ptr module_uptr = std::make_unique<module_type>();
    return set_main_module<module_type>(std::move(module_uptr));
}

template <typename application_base_type, typename application_type>
template <class ApplicationType>
    requires requires(ApplicationType app)
{
    { *(app.logger()) } -> std::convertible_to<spdlog::logger&>;
}
void multi_task<application_base_type, application_type>::notify_exception_(ApplicationType& app,
                                                                            std::string_view context,
                                                                            const std::exception* ex)
{
    spdlog::logger& logger = *(app.logger());
    std::string error_msg;
    if (ex)
        error_msg = std::format("{}:\n{}", context, ex->what());
    else
        error_msg = std::format("{}.", context);
    SPDLOG_LOGGER_CRITICAL(&logger, error_msg);
}

template <typename application_base_type, typename application_type>
template <class ApplicationType>
    requires (!requires(ApplicationType app)
             {
                 { *(app.logger()) } -> std::convertible_to<spdlog::logger&>;
             })
void multi_task<application_base_type, application_type>::notify_exception_(ApplicationType&,
                                                                            std::string_view context,
                                                                            const std::exception* ex)
{
    std::string error_msg;
    if (ex)
        error_msg = std::format("{}:\n{}", context, ex->what());
    else
        error_msg = std::format("{}.", context);
    private_::notify_error_message_to_cerr_(error_msg);
}

}
}
}
