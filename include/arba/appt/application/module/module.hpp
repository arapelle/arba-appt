#pragma once

#include "module_interface.hpp"
#include <arba/rsce/resource_manager.hpp>
#include <arba/evnt/evnt.hpp>

inline namespace arba
{
namespace appt
{

template <class app_type>
class module : public module_interface
{
public:
    template <typename other_application_type>
    using rebind_t = module;

    using application_type = app_type;

    explicit module(std::string_view name = std::string_view()) : module_interface(name), application_(nullptr) {}
    virtual ~module() override = default;

    inline const application_type& app() const { return *application_; }
    inline application_type& app() { return *application_; }
    void set_app(application_type& app);

    virtual void init() override;

protected:
    const evnt::event_box& event_box() const { return event_box_; }
    evnt::event_box& event_box() { return event_box_; }
    const evnt::event_manager& event_manager() const { return event_manager_; }
    evnt::event_manager& event_manager() { return event_manager_; }

private:
    application_type* application_;
    evnt::event_box event_box_;
    evnt::event_manager event_manager_;
};

// Template methods implementation:

template <class app_type>
void module<app_type>::set_app(module::application_type &app)
{
    if (application_ && application_ != &app)
        throw std::runtime_error("Module is already linked to an application.");
    application_ = &app;
}

template <class app_type>
void module<app_type>::init()
{
    app().event_manager().connect(event_box_);
}

}
}
