#pragma once

#include "application_module.hpp"
#include <memory>

inline namespace arba
{
namespace appt
{

template <typename application_base_type, typename application_type = void>
class multi_user;

template <typename application_base_type>
class multi_user<application_base_type> : public application_base_type
{
public:
    using application_base_type::application_base_type;

    template <typename other_application_type>
    using rebind_t = multi_user<application_base_type, other_application_type>;

private:
};

template <typename application_base_type, typename application_type>
class multi_user : public multi_user<typename application_base_type::rebind_t<application_type>>
{
    using base_ = multi_user<typename application_base_type::rebind_t<application_type>>;

public:
    using base_::multi_user;
};

}
}
