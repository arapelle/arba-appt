#pragma once

#include "basic_module.hpp"
#include <arba/rsce/resource_manager.hpp>
#include <arba/evnt/evnt.hpp>

inline namespace arba
{
namespace appt
{

template <class ApplicationType, class ModuleType = void>
class module;

template <class ApplicationType, class ModuleType>
class module : public basic_module<ApplicationType, ModuleType>
{
private:
    using base_ = basic_module<ApplicationType, ModuleType>;

public:
    template <typename OtherModuleType>
    using rebind_t = module<ApplicationType, OtherModuleType>;

    using base_::base_;
    virtual ~module() override = default;

    virtual void init() override;

    const evnt::event_box& event_box() const { return event_box_; }
    evnt::event_box& event_box() { return event_box_; }
    const evnt::event_manager& event_manager() const { return event_manager_; }
    evnt::event_manager& event_manager() { return event_manager_; }

private:
    evnt::event_box event_box_;
    evnt::event_manager event_manager_;
};

// Template methods implementation:

template <class ApplicationType, class ModuleType>
void module<ApplicationType, ModuleType>::init()
{
    base_::init();
    this->app().event_manager().connect(event_box_);
}

}
}
