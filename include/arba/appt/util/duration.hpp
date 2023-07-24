#pragma once

#include <chrono>
#include <cstdint>

inline namespace arba
{
namespace appt
{

class duration : public std::chrono::steady_clock::duration
{
private:
    using base_ = std::chrono::steady_clock::duration;

public:
    using base_::duration;
    using base_::operator=;

    inline explicit duration(const base_& arg) : base_(arg) {}

    template <class rep = double>
    inline rep to_seconds() const
    {
        return std::chrono::duration_cast<std::chrono::duration<rep>>(*this).count();
    }

    template <class rep = double>
    inline rep to_hours() const
    {
        return std::chrono::duration_cast<std::chrono::duration<rep, std::chrono::hours::period>>(*this).count();
    }

    template <class rep = std::chrono::milliseconds::rep>
    inline rep to_milliseconds() const
    {
        return std::chrono::duration_cast<std::chrono::duration<rep, std::milli>>(*this).count();
    }
};

class seconds : public duration
{
public:
    using duration::duration;
    using duration::operator=;

    inline seconds(const duration& arg) : duration(arg) {}
    inline operator double() const { return duration::to_seconds(); }
};

class hours : public duration
{
public:
    using duration::duration;
    using duration::operator=;

    inline hours(const duration& arg) : duration(arg) {}
    inline operator double() const { return duration::to_hours(); }
};

}
}
