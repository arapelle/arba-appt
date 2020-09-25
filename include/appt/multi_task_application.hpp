#pragma once

#include "application_module.hpp"
#include "application.hpp"
#include <memory>
#include <thread>

inline namespace arba
{
namespace appt
{

class multi_task_application : public application
{
public:
    using application::application;

    template <typename module_type>
    requires std::is_base_of_v<application_module_interface, module_type> && (!std::is_abstract_v<module_type>)
    void set_main_module(std::unique_ptr<module_type>&& module_uptr);

    template <typename module_type>
    requires std::is_base_of_v<application_module_interface, module_type> && (!std::is_abstract_v<module_type>)
    module_type& add_module(std::unique_ptr<module_type>&& module_uptr);

    void init();
    void run();
    void stop_side_modules();

private:
    using application_module_interface_uptr = std::unique_ptr<application_module_interface>;
    std::vector<std::pair<application_module_interface_uptr, std::jthread>> side_modules_;
    application_module_interface_uptr main_module_;
    std::mutex mutex_;
};

template <typename module_type>
requires std::is_base_of_v<application_module_interface, module_type> && (!std::is_abstract_v<module_type>)
void multi_task_application::set_main_module(std::unique_ptr<module_type>&& module_uptr)
{
    main_module_ = std::move(module_uptr);
}

template <typename module_type>
requires std::is_base_of_v<application_module_interface, module_type> && (!std::is_abstract_v<module_type>)
module_type& multi_task_application::add_module(std::unique_ptr<module_type>&& module_uptr)
{
    module_type* module_ptr = module_uptr.get();
    side_modules_.emplace_back(std::move(module_uptr), std::jthread());
    return *module_ptr;
}

}
}
