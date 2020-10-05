#pragma once 

#include "decorator/toolkit.hpp"
#include "program.hpp"
#include <string>

inline namespace arba
{
namespace appt
{

class application : public adec::toolkit<program>
{
public:
    template <typename other_application_type>
    using rebind_t = application;

    using toolkit<program>::toolkit;
};

}
}
