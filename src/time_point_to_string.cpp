#include <appt/util/time_point_to_string.hpp>
#include <sstream>
#include <iomanip>

inline namespace arba
{
namespace appt
{

std::string to_string_Ymd_HMS(std::chrono::system_clock::time_point time_point)
{
    std::time_t timer = std::chrono::system_clock::to_time_t(time_point);
    std::tm* tm = std::localtime(&timer);
    std::ostringstream stream;
    stream << std::put_time(tm, "%Y%m%d_%H%M%S");
    return stream.str();
}

std::string to_string_Ymd_HMS_ms(std::chrono::system_clock::time_point time_point)
{
    // get number of milliseconds for the current second (remainder after division into seconds)
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch()) % 1000;
    // convert to std::time_t in order to convert to std::tm (broken time)
    std::time_t timer = std::chrono::system_clock::to_time_t(time_point);
    std::tm* tm = std::localtime(&timer);
    std::ostringstream stream;
    stream << std::put_time(tm, "%Y%m%d_%H%M%S") << '_' << std::setfill('0') << std::setw(3) << ms.count();
    return stream.str();
}

std::string to_string_Ymd_HMS_mcs(std::chrono::system_clock::time_point time_point)
{
    // get number of microseconds for the current second (remainder after division into seconds)
    std::chrono::microseconds mcs = std::chrono::duration_cast<std::chrono::microseconds>(time_point.time_since_epoch()) % 1000000;
    // convert to std::time_t in order to convert to std::tm (broken time)
    std::time_t timer = std::chrono::system_clock::to_time_t(time_point);
    std::tm* tm = std::localtime(&timer);
    std::ostringstream stream;
    stream << std::put_time(tm, "%Y%m%d_%H%M%S") << '_' << std::setfill('0') << std::setw(6) << mcs.count();
    return stream.str();
}



}
}
