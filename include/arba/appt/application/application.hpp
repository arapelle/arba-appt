#pragma once

#include "basic_application.hpp"
#include "decorator/toolkit.hpp"

#include <string>

inline namespace arba
{
namespace appt
{

template <class ApplicationType = void>
class application : public adec::toolkit<basic_application<ApplicationType>>
{
public:
    template <typename OtherType>
    using rebind_t = application<OtherType>;

    using toolkit<basic_application<ApplicationType>>::toolkit;
};

} // namespace appt
} // namespace arba
