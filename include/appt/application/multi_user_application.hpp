#pragma once

#include "decorator/multi_user.hpp"
#include <appt/user/user_manager.hpp>
#include "application.hpp"
#include <memory>

inline namespace arba
{
namespace appt
{

template <typename user_type, typename application_type = void>
using multi_user_application = adec::multi_user<user_type, application, application_type>;

}
}
