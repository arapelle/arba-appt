#pragma once

#include <arba/meta/type_traits/enum.hpp>

#include <cstdint>
#include <cstdlib>

inline namespace arba
{
namespace appt
{

class execution_statuses;

class execution_status : public meta::enumerator<execution_status, execution_statuses, int8_t>
{
    ARBA_META_ENUMERATOR(execution_status, EXIT_SUCCESS)
};

class execution_statuses : public meta::enumeration<execution_status, execution_statuses>
{
public:
    static constexpr execution_status success = make_enumerator(EXIT_SUCCESS);
    static constexpr execution_status failure = make_enumerator(EXIT_FAILURE);
    static constexpr execution_status ready = make_enumerator((success > failure ? success : failure).value() + 1);
    static constexpr execution_status executing = make_enumerator(ready.value() + 1);

    static constexpr std::array<std::string_view, 4> enumerator_names{ "success", "failure", "ready", "executing" };

    static constexpr std::size_t enumerator_index_offset() { return (success < failure ? success : failure).value(); }
    static constexpr std::size_t enumerator_index_factor() { return 1; }
};

} // namespace appt
} // namespace arba
