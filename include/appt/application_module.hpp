#pragma once

#include "application.hpp"
#include <thread>

inline namespace arba
{
namespace appt
{

class application_module_interface
{
public:
    application_module_interface();
    virtual ~application_module_interface() = default;

    virtual void init() {}
    virtual void run() = 0;

    struct jrunner
    {
        application_module_interface*const module_ptr;
        ~jrunner();
        void operator()(std::stop_token s_token);
    };
    jrunner jthread_runner();

private:
    std::stop_token stop_token_;
};

template <class app_type = application>
requires std::is_base_of_v<application, app_type>
class application_module : public application_module_interface
{
public:
    using application_type = app_type;

    explicit application_module(application_type& app);
    virtual ~application_module() override = default;

    const application_type& app() const { return *application_; }
    application_type& app() { return *application_; }

protected:
    const evnt::event_box& event_box() const { return event_box_; }
    evnt::event_box& event_box() { return event_box_; }

private:
    application_type* application_;
    evnt::event_box event_box_;
};

// Template methods implementation:

template <class app_type>
application_module<app_type>::application_module(application_type& app)
    : application_(&app)
{
    application_->event_manager().connect(event_box_);
}

}
}
