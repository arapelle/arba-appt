#pragma once

#include "module_interface.hpp"
#include <arba/core/debug/assert.hpp>

inline namespace arba
{
namespace appt
{

template <class ApplicationType, class ModuleType = void>
class basic_module;

template <class ApplicationType>
class basic_module<ApplicationType, void> : public module_interface
{
public:
    template <typename OtherModuleType>
    using rebind_t = basic_module<ApplicationType, OtherModuleType>;

    using application_type = ApplicationType;

    using module_interface::module_interface;
    virtual ~basic_module() override = default;

    inline const application_type& app() const { return *application_; }
    inline application_type& app() { return *application_; }
    void set_app(application_type& app);

    virtual void init() override;

private:
    application_type* application_ = nullptr;
};

// Template methods implementation:

template <class ApplicationType>
void basic_module<ApplicationType>::set_app(application_type& app)
{
    if (application_ && application_ != &app) [[unlikely]]
        throw std::runtime_error("Module is already linked to an application.");
    application_ = &app;
}

template <class ApplicationType>
void basic_module<ApplicationType>::init()
{
    ARBA_ASSERT(application_ != nullptr);
}


template <class ApplicationType, class ModuleType>
class basic_module : public basic_module<ApplicationType>
{
private:
    using base_ = basic_module<ApplicationType>;

public:
    using base_::base_;

protected:
    using self_type_ = ModuleType;

    [[nodiscard]] inline const self_type_& self_() const noexcept { return static_cast<self_type_&>(*this); }
    [[nodiscard]] inline self_type_& self_() noexcept { return static_cast<self_type_&>(*this); }
};

}
}
