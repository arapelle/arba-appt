#include <arba/appt/util/delta_time.hpp>
#include <arba/appt/application/module/decorator/loop.hpp>
#include <gtest/gtest.h>
#include <type_traits>
#include <cstdlib>

using namespace std::chrono_literals;

static_assert(std::is_integral_v<appt::mdec::loop_base::duration::rep>);

TEST(delta_time, dt_seconds_constructor__loop_base_duration__ok)
{
    appt::mdec::loop_base::duration dur(1500ms);
    arba::appt::dt::seconds dt(dur);
    double value = dt;
    ASSERT_DOUBLE_EQ(value, 1.5);
    ASSERT_EQ(static_cast<const void*>(&dt.as_duration()), static_cast<const void*>(&dt));
}

TEST(delta_time, dt_hours_constructor__loop_base_duration__ok)
{
    appt::mdec::loop_base::duration dur(90min);
    arba::appt::dt::hours dt(dur);
    double value = dt;
    ASSERT_DOUBLE_EQ(value, 1.5);
    ASSERT_EQ(static_cast<const void*>(&dt.as_duration()), static_cast<const void*>(&dt));
}
