#include <appt/multi_task_application.hpp>
#include <iostream>

inline namespace arba
{
namespace appt
{

void multi_task_application::init()
{
    for (auto& entry : side_modules_)
        entry.first->init();
}

void multi_task_application::stop_side_modules()
{
    std::lock_guard lock(mutex_);
    for (auto& entry : side_modules_)
        entry.second.request_stop();
}

void multi_task_application::run()
{
    for (auto& entry : side_modules_)
    {
        application_module_interface* module_ptr = entry.first.get();
        entry.second = std::jthread(module_ptr->jthread_runner());
    }

    if (main_module_)
    {
        main_module_->run();
        stop_side_modules();
    }

    for (auto& entry : side_modules_)
        entry.second.join();
}

}
}
