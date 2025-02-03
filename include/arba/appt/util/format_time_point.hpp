#pragma once

#include <chrono>
#include <string>

inline namespace arba
{
namespace appt
{

// "%Y-%m-%d %H:%M:%S" -> "2023-25-10 12:04:17"
std::string format_log_Ymd_HMS(std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now());

// "%Y-%m-%d %H:%M:%S.ms" -> "2023-25-10 12:04:17.256"
std::string format_log_Ymd_HMS_ms(std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now());

// "%Y-%m-%d %H:%M:%S.mcs" -> "2023-25-10 12:04:17.256652"
std::string format_log_Ymd_HMS_mcs(std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now());

// "%Y%m%d_%H%M%S" -> "20232510_120417"
std::string
format_filename_Ymd_HMS(std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now());

// "%Y%m%d_%H%M%S_ms" -> "20232510_120417_256"
std::string
format_filename_Ymd_HMS_ms(std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now());

// "%Y%m%d_%H%M%S_mcs" -> "20232510_120417_256652"
std::string
format_filename_Ymd_HMS_mcs(std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now());

} // namespace appt
} // namespace arba
