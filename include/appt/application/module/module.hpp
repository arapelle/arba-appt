#pragma once

#include <appt/application/application.hpp>
#include <thread>

inline namespace arba
{
namespace appt
{

class module_interface
{
public:
    module_interface();
    module_interface(std::string name);
    virtual ~module_interface() = default;

    const std::string& name() const { return name_; }
    virtual void init() {}
    virtual void run() = 0;

public:
    struct jrunner
    {
        module_interface*const module_ptr;
        ~jrunner();
        void operator()(std::stop_token s_token);
    };
    jrunner jthread_runner();

protected:
    inline const std::stop_token& stop_token() const { return stop_token_; }
    inline std::stop_token& stop_token() { return stop_token_; }

private:
    std::size_t new_module_index_()
    {
        static std::size_t index = 0;
        return index++;
    }

private:
    std::string name_;
    std::stop_token stop_token_;
};

template <class app_type = application>
requires std::is_base_of_v<application, app_type>
class module : public module_interface
{
public:
    template <typename other_application_type>
    using rebind_t = module;

    using application_type = app_type;

    module() : application_(nullptr) {}
    explicit module(std::string name) : module_interface(std::move(name)), application_(nullptr) {}
    explicit module(application_type& app);
    module(std::string name, application_type& app);
    virtual ~module() override = default;

    inline const application_type& app() const { return *application_; }
    inline application_type& app() { return *application_; }
    void set_app(application_type& app);

protected:
    const evnt::event_box& event_box() const { return event_box_; }
    evnt::event_box& event_box() { return event_box_; }

private:
    application_type* application_;
    evnt::event_box event_box_;
};

// Template methods implementation:

template <class app_type>
module<app_type>::module(application_type& app)
    : application_(&app)
{
    application_->event_manager().connect(event_box_);
}

template <class app_type>
module<app_type>::module(std::string name, application_type& app)
    : module_interface(std::move(name)), application_(&app)
{
    application_->event_manager().connect(event_box_);
}

template <class app_type>
void module<app_type>::set_app(module::application_type &app)
{
    if (application_ && application_ != &app)
        throw std::runtime_error("Module is already linked to an application.");
    application_ = &app;
    app.event_manager().connect(event_box_);
}

}
}
