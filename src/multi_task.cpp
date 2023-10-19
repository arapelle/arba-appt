#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/util/format_time_point.hpp>
#include <iostream>

inline namespace arba
{
namespace appt
{
inline namespace adec // application_decorator
{
namespace private_
{

void log_error_message_to_cerr_(std::string_view error_msg)
{
    std::cerr << "[critical][" << format_log_Ymd_HMS_mcs() << "]: " << error_msg << std::endl;
}

}
}
}
}
