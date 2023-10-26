#pragma once

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

}
}
