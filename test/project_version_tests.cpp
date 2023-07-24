#include <arba/appt/version.hpp>
#include <gtest/gtest.h>
#include <cstdlib>

TEST(project_version_tests, test_version_macros)
{
    ASSERT_EQ(ARBA_APPT_VERSION_MAJOR, 0);
    ASSERT_EQ(ARBA_APPT_VERSION_MINOR, 12);
    ASSERT_EQ(ARBA_APPT_VERSION_PATCH, 0);
    ASSERT_STREQ(ARBA_APPT_VERSION, "0.12.0");
}
