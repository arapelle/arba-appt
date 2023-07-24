#pragma once

#include <chrono>
#include <string>

inline namespace arba
{
namespace appt
{

std::string to_string_Ymd_HMS(std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now());
std::string to_string_Ymd_HMS_ms(std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now());
std::string to_string_Ymd_HMS_mcs(std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now());

}
}
