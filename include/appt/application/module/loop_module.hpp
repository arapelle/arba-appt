#pragma once

#include "decorator/loop.hpp"
#include "module.hpp"

inline namespace arba
{
namespace appt
{

template <typename app_type = application, typename module_type = void>
class loop_module;

template <typename app_type>
class loop_module<app_type> : public mdec::loop<module<app_type>, void>
{
private:
    using base_ = mdec::loop<module<app_type>, void>;

public:
    template <typename other_module_type>
    using rebind_t = loop_module<other_module_type, app_type>;

public:
    using base_::base_;
    virtual ~loop_module() override = default;
};

template <typename app_type, typename module_type>
class loop_module : public mdec::loop<module<app_type>, module_type>
{
private:
    using base_ = mdec::loop<module<app_type>, module_type>;

public:
    using base_::base_;
    virtual ~loop_module() override = default;
};

}
}
