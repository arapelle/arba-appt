#include <arba/appt/util/format_time_point.hpp>
#include <arba/appt/util/logging/log_critical_message.hpp>

#include <iostream>

inline namespace arba
{
namespace appt
{

void log_critical_message_to_cerr(std::string_view error_msg)
{
    std::cerr << "[critical][" << format_log_Ymd_HMS_mcs() << "]: " << error_msg << std::endl;
}

} // namespace appt
} // namespace arba
