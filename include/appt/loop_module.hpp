#pragma once

#include "loop.hpp"
#include "module.hpp"

inline namespace arba
{
namespace appt
{

template <typename module_type, typename app_type = application>
class loop_module : public mdec::loop<module<app_type>, module_type>
{
public:
    using mdec::loop<module<app_type>, module_type>::loop;
};

}
}
