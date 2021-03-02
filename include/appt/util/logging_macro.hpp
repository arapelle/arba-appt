#pragma once

#include <spdlog/spdlog.h>

inline namespace arba
{
namespace appt
{

#if (defined(__GNUC__))
#define ARBA_APPT_LOGGER_TRACE(logger) SPDLOG_LOGGER_TRACE(logger, __PRETTY_FUNCTION__)
#else
#define ARBA_APPT_LOGGER_TRACE(logger) SPDLOG_LOGGER_TRACE(logger, __func__)
#endif

}
}
