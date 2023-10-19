#include <arba/appt/application/execution_status.hpp>
#include <gtest/gtest.h>
#include <format>
#include <cstdlib>

TEST(execution_status_tests, compare_values)
{
    ASSERT_NE(appt::execution_success, appt::execution_failure);
    ASSERT_EQ(appt::execution_success, EXIT_SUCCESS);
    ASSERT_EQ(appt::execution_failure, EXIT_FAILURE);
    ASSERT_EQ(appt::execution_success, 0);
    ASSERT_GT(appt::ready, appt::execution_success);
    ASSERT_GT(appt::ready, appt::execution_failure);
    ASSERT_GT(appt::executing, appt::ready);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int exit_value = RUN_ALL_TESTS();
    return exit_value;
}
