#pragma once

#include "decorator/multi_task.hpp"
#include <appt/application/module/module.hpp>
#include <memory>
#include <thread>

inline namespace arba
{
namespace appt
{

template <typename application_type = void>
using multi_task_application = adec::multi_task<application, application_type>;

}
}
