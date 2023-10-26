#include <arba/appt/util/format_time_point.hpp>
#include <gtest/gtest.h>
#include <format>
#include <cstdlib>

std::chrono::system_clock::time_point make_time_point(std::string_view date_str, unsigned ms = 0, unsigned mcs = 0)
{
    std::chrono::system_clock::time_point time_point;
    std::stringstream stream(date_str.data());
    std::tm tm = {};
    stream >> std::get_time(&tm, "%Y/%m/%d %T");
    time_point = std::chrono::system_clock::from_time_t(std::mktime(&tm))
                 + std::chrono::milliseconds(ms) + std::chrono::microseconds(mcs);
    return time_point;
}

TEST(format_time_point_tests, test_format_filename)
{
    std::chrono::system_clock::time_point time_point = make_time_point("2014/01/09 12:35:34");
    ASSERT_EQ(appt::format_filename_Ymd_HMS(time_point), "20140109_123534");
    ASSERT_EQ(appt::format_filename_Ymd_HMS_ms(time_point), "20140109_123534_000");
    ASSERT_EQ(appt::format_filename_Ymd_HMS_mcs(time_point), "20140109_123534_000000");

    time_point = make_time_point("2014/01/09 12:35:34", 123);
    ASSERT_EQ(appt::format_filename_Ymd_HMS(time_point), "20140109_123534");
    ASSERT_EQ(appt::format_filename_Ymd_HMS_ms(time_point), "20140109_123534_123");
    ASSERT_EQ(appt::format_filename_Ymd_HMS_mcs(time_point), "20140109_123534_123000");

    time_point = make_time_point("2014/01/09 12:35:34", 123, 786);
    ASSERT_EQ(appt::format_filename_Ymd_HMS(time_point), "20140109_123534");
    ASSERT_EQ(appt::format_filename_Ymd_HMS_ms(time_point), "20140109_123534_123");
    ASSERT_EQ(appt::format_filename_Ymd_HMS_mcs(time_point), "20140109_123534_123786");
}

TEST(format_time_point_tests, test_format_log)
{
    std::chrono::system_clock::time_point time_point = make_time_point("2014/01/09 12:35:34");
    ASSERT_EQ(appt::format_log_Ymd_HMS(time_point), "2014-01-09 12:35:34");
    ASSERT_EQ(appt::format_log_Ymd_HMS_ms(time_point), "2014-01-09 12:35:34.000");
    ASSERT_EQ(appt::format_log_Ymd_HMS_mcs(time_point), "2014-01-09 12:35:34.000000");

    time_point = make_time_point("2014/01/09 12:35:34", 123);
    ASSERT_EQ(appt::format_log_Ymd_HMS(time_point), "2014-01-09 12:35:34");
    ASSERT_EQ(appt::format_log_Ymd_HMS_ms(time_point), "2014-01-09 12:35:34.123");
    ASSERT_EQ(appt::format_log_Ymd_HMS_mcs(time_point), "2014-01-09 12:35:34.123000");


    time_point = make_time_point("2014/01/09 12:35:34", 123, 786);
    ASSERT_EQ(appt::format_log_Ymd_HMS(time_point), "2014-01-09 12:35:34");
    ASSERT_EQ(appt::format_log_Ymd_HMS_ms(time_point), "2014-01-09 12:35:34.123");
    ASSERT_EQ(appt::format_log_Ymd_HMS_mcs(time_point), "2014-01-09 12:35:34.123786");
}
