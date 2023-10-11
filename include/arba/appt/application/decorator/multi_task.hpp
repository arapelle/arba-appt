#pragma once

#include <arba/appt/application/module/module.hpp>
#include <memory>
#include <thread>

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

    template <typename module_type>
    requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
    module_type& set_main_module(std::unique_ptr<module_type>&& module_uptr);

    template <typename module_type>
    requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
    module_type& add_module(std::unique_ptr<module_type>&& module_uptr);

    void init();
    void run();
    void stop_side_modules();

private:
    using module_interface_uptr = std::unique_ptr<module_interface>;
    std::vector<std::pair<module_interface_uptr, std::jthread>> side_modules_;
    module_interface_uptr main_module_;
    std::mutex mutex_;
};

template <typename application_base_type>
void multi_task<application_base_type>::init()
{
    if (main_module_)
        main_module_->init();
    for (auto& entry : side_modules_)
        entry.first->init();
}

template <typename application_base_type>
void multi_task<application_base_type>::run()
{
    for (auto& entry : side_modules_)
    {
        module_interface* module_ptr = entry.first.get();
        entry.second = std::jthread(module_ptr->jthread_runner());
    }

    if (main_module_)
        main_module_->run();

    for (auto& entry : side_modules_)
        entry.second.join();
}

template <typename application_base_type>
void multi_task<application_base_type>::stop_side_modules()
{
    std::lock_guard lock(mutex_);
    for (auto& entry : side_modules_)
        entry.second.request_stop();
}

template <typename application_type>
template <typename module_type>
requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_type>::set_main_module(std::unique_ptr<module_type>&& module_uptr)
{
    if (&module_uptr->app() == nullptr)
        throw std::invalid_argument("The module must be linked to this application before adding it.");
    module_type* module_ptr = module_uptr.get();
    main_module_ = std::move(module_uptr);
    return *module_ptr;
}

template <typename application_type>
template <typename module_type>
requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_type>::add_module(std::unique_ptr<module_type>&& module_uptr)
{
    if (&module_uptr->app() == nullptr)
        throw std::invalid_argument("The module must be linked to this application before adding it.");
    module_type* module_ptr = module_uptr.get();
    side_modules_.emplace_back(std::move(module_uptr), std::jthread());
    return *module_ptr;
}


template <typename application_base_type, typename application_type>
class multi_task : public multi_task<typename application_base_type::template rebind_t<application_type>>
{
    using base_ = multi_task<typename application_base_type::template rebind_t<application_type>>;

public:
    using base_::base_;

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
};

template <typename application_base_type, typename application_type>
template <typename module_type>
requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::set_main_module(std::unique_ptr<module_type>&& module_uptr)
{
    module_uptr->set_app(this->self_());
    return this->base_::template set_main_module<module_type>(std::move(module_uptr));
}

template <typename application_base_type, typename application_type>
template <typename module_type>
requires std::is_base_of_v<module_interface, module_type> && (!std::is_abstract_v<module_type>)
module_type& multi_task<application_base_type, application_type>::add_module(std::unique_ptr<module_type>&& module_uptr)
{
    module_uptr->set_app(this->self_());
    return this->base_::template add_module<module_type>(std::move(module_uptr));
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

}
}
}
