#pragma once 

#include "decorator/toolkit.hpp"
#include "program.hpp"
#include <string>

inline namespace arba
{
namespace appt
{

template <class ApplicationType = void>
class application : public adec::toolkit<program<ApplicationType>>
{
public:
    template <typename OtherApplicationType>
    using rebind_t = application<OtherApplicationType>;

    using toolkit<program<ApplicationType>>::toolkit;
};

}
}
