#pragma once

#include <arba/meta/type_traits/enum.hpp>

#include <cstdint>
#include <cstdlib>

inline namespace arba
{
namespace appt
{

enum execution_status : int8_t
{
    execution_success = EXIT_SUCCESS,
    execution_failure = EXIT_FAILURE,
    ready = (execution_success > execution_failure ? execution_success : execution_failure) + 1,
    executing,
};

namespace neo
{
class execution_statuses;

class execution_status : public meta::enumerator<int8_t, execution_statuses>
{
private:
    using base_ = meta::enumerator<int8_t, execution_statuses>;

public:
    constexpr execution_status() {}
    consteval execution_status(const base_& val) : base_(val) {}
    constexpr auto operator<=>(const execution_status& other) const noexcept = default;//{ return this->value() <=> value; }
    constexpr auto operator<=>(int8_t value) const noexcept { return this->value() <=> value; }
};


class execution_statuses : public meta::enumeration<execution_status, execution_statuses>
{
public:
    static constexpr execution_status success = make_enumerator(EXIT_SUCCESS);
    static constexpr execution_status failure = make_enumerator(EXIT_FAILURE);
    static constexpr execution_status ready = make_enumerator((success > failure ? success : failure) + 1);
    static constexpr execution_status executing = make_enumerator(ready + 1);
};
} // namespace neo

} // namespace appt
} // namespace arba
