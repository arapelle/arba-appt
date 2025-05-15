#include <arba/appt/application/execution_status.hpp>

#include <gtest/gtest.h>

#include <cstdlib>
#include <format>

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

TEST(execution_status_tests, compare_values_)
{
    static_assert(std::totally_ordered_with<appt::neo::execution_status, int8_t>);
    ASSERT_NE(appt::neo::execution_statuses::success, appt::neo::execution_statuses::failure);
    ASSERT_EQ(appt::neo::execution_statuses::success, EXIT_SUCCESS);
    ASSERT_EQ(appt::neo::execution_statuses::failure, EXIT_FAILURE);
    const int res = appt::neo::execution_statuses::success;
    ASSERT_EQ(res, 0);
    ASSERT_GT(appt::neo::execution_statuses::ready, appt::neo::execution_statuses::success);
    ASSERT_GT(appt::neo::execution_statuses::ready, appt::neo::execution_statuses::failure);
    ASSERT_GT(appt::neo::execution_statuses::executing, appt::neo::execution_statuses::ready);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int exit_value = RUN_ALL_TESTS();
    return exit_value;
}
