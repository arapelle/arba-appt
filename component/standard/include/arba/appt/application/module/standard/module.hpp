#pragma once

#include <arba/appt/application/module/basic_module.hpp>

#include <arba/evnt/evnt.hpp>
#include <arba/rsce/resource_manager.hpp>

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
    template <typename OtherType>
    using rebind_t = module<ApplicationType, OtherType>;

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
    if (event_box_.listened_event_manager() == nullptr) [[likely]]
        this->app().event_manager().connect(event_box_);
}

} // namespace appt
} // namespace arba
