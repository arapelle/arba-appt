#pragma once

#include "decorator/multi_user.hpp"
#include "module.hpp"
#include <appt/user/user_set.hpp>
#include <memory>

inline namespace arba
{
namespace appt
{

template <typename user_type, typename user_sptr_hash, typename app_type = application>
class multi_user_module : public mdec::multi_user<user_type, user_sptr_hash, module<app_type>>
{
    using base_ = mdec::multi_user<user_type, user_sptr_hash, module<app_type>>;

public:
    template <typename other_module_type>
    using rebind_t = multi_user_module;

public:
    using base_::multi_user;
    virtual ~multi_user_module() override = default;
};

}
}
