#pragma once

#include <spdlog/spdlog.h>

#include <source_location>

inline namespace arba
{
namespace appt
{

#define ARBA_APPT_LOGGER_TRACE(logger) SPDLOG_LOGGER_TRACE(logger, std::source_location::current().function_name())

} // namespace appt
} // namespace arba
