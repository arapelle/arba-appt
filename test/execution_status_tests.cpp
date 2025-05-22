#include <arba/appt/application/execution_status.hpp>

#include <gtest/gtest.h>

#include <cstdlib>
#include <format>

static_assert(std::totally_ordered<appt::execution_status>);
static_assert(std::totally_ordered_with<appt::execution_status, int8_t>);

TEST(execution_status_tests, compare_values)
{
    ASSERT_NE(appt::execution_statuses::success, appt::execution_statuses::failure);
    ASSERT_EQ(appt::execution_statuses::success, EXIT_SUCCESS);
    ASSERT_EQ(appt::execution_statuses::failure, EXIT_FAILURE);
    const int res = appt::execution_statuses::success;
    ASSERT_EQ(res, 0);
    ASSERT_GT(appt::execution_statuses::ready, appt::execution_statuses::success);
    ASSERT_GT(appt::execution_statuses::ready, appt::execution_statuses::failure);
    ASSERT_GT(appt::execution_statuses::executing, appt::execution_statuses::ready);
}

TEST(execution_status_tests, compare_names)
{
    ASSERT_EQ(appt::execution_statuses::success.name(), "success");
    ASSERT_EQ(appt::execution_statuses::failure.name(), "failure");
    ASSERT_EQ(appt::execution_statuses::ready.name(), "ready");
    ASSERT_EQ(appt::execution_statuses::executing.name(), "executing");
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int exit_value = RUN_ALL_TESTS();
    return exit_value;
}
