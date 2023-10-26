#pragma once

#include <string_view>

inline namespace arba
{
namespace appt
{

void log_critical_message_to_cerr(std::string_view error_msg);

}
}
