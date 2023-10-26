#pragma once 

#include "decorator/toolkit.hpp"
#include "basic_application.hpp"
#include <string>

inline namespace arba
{
namespace appt
{

template <class ApplicationType = void>
class application : public adec::toolkit<basic_application<ApplicationType>>
{
public:
    template <typename OtherApplicationType>
    using rebind_t = application<OtherApplicationType>;

    using toolkit<basic_application<ApplicationType>>::toolkit;
};

}
}
