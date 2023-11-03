#pragma once

#include <arba/core/chrono/duration.hpp>
#include <cstdint>

inline namespace arba
{
namespace appt
{
namespace dt  // delta_time
{

class seconds : private core::seconds_f64
{
public:
    using duration_type = core::seconds_f64;

    using duration_type::duration_type;
    template <class RepT, class PeriodT>
    inline seconds(const std::chrono::duration<RepT, PeriodT>& arg)
        : duration_type(arg)
    {}

    inline operator double() const { return this->count(); }
    [[nodiscard]] inline const duration_type& as_duration() const noexcept
    {
        return static_cast<const duration_type&>(*this);
    }
};

class hours : private core::hours_f64
{
public:
    using duration_type = core::hours_f64;

    using duration_type::duration_type;
    template <class rep, class period>
    inline hours(const std::chrono::duration<rep, period>& arg)
        : duration_type(arg)
    {}

    inline operator double() const { return this->count(); }
    [[nodiscard]] inline const duration_type& as_duration() const noexcept
    {
        return static_cast<const duration_type&>(*this);
    }
};

}
}
}