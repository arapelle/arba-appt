#pragma once

#include "user_manager.hpp"
#include "application.hpp"
#include <memory>

inline namespace arba
{
namespace appt
{
namespace adec
{

template <typename user_type, typename application_base_type, typename application_type = void>
class multi_user;

template <typename user_type, typename application_base_type>
class multi_user<user_type, application_base_type> : public application_base_type
{
private:
    using user_sptr = std::shared_ptr<user_type>;
    using user_manager_type = user_manager<user_type>;

public:
    using application_base_type::application_base_type;

    template <typename other_application_type>
    using rebind_t = multi_user<user_type, application_base_type, other_application_type>;

    inline const user_manager_type& usr_manager() const { return user_manager_; }
    inline user_manager_type& usr_manager() { return user_manager_; }

private:
    user_manager_type user_manager_;
};

template <typename user_type, typename application_base_type, typename application_type>
class multi_user : public multi_user<user_type, typename application_base_type::rebind_t<application_type>>
{
    using base_ = multi_user<user_type, typename application_base_type::rebind_t<application_type>>;

public:
    using base_::multi_user;
};

}

template <typename user_type, typename application_type = void>
using multi_user_application = adec::multi_user<user_type, application, application_type>;

}
}
