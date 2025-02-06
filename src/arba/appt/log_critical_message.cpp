#include <arba/appt/util/logging/log_critical_message.hpp>

#include <arba/stdx/chrono/format_time_point.hpp>

#include <iostream>

inline namespace arba
{
namespace appt
{

void log_critical_message_to_cerr(std::string_view error_msg)
{
    std::cerr << "[critical][" << stdx::format_Ymd_HMS_mcs() << "]: " << error_msg << std::endl;
}

} // namespace appt
} // namespace arba
