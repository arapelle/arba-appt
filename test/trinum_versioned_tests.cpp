#include <arba/appt/application/decorator/versioned/trinum_versioned.hpp>
#include <arba/appt/application/basic_application.hpp>
#include <gtest/gtest.h>
#include <type_traits>
#include <cstdlib>


class ut_application : public appt::trinum_versioned<appt::basic_application<>, ut_application>
{
    using base_ = appt::trinum_versioned<appt::basic_application<>, ut_application>;

public:
    using base_::base_;
};

class ut_custom_application
    : public appt::trinum_versioned<appt::basic_application<>, ut_custom_application>
{
    using base_ = appt::trinum_versioned<appt::basic_application<>, ut_custom_application>;

public:
    using base_::base_;

    [[nodiscard]] inline static constexpr auto make_trinum_version()
    {
        return core::trinum_version<1, 2, 4>{};
    }
};

TEST(trinum_versioned, test_default)
{
    ASSERT_EQ(ut_application::trinum_version(), (core::trinum_version<0, 1, 0>{}));
    ASSERT_EQ(ut_application::trinum_version().major(), 0);
    ASSERT_EQ(ut_application::trinum_version().minor(), 1);
    ASSERT_EQ(ut_application::trinum_version().patch(), 0);
}

TEST(trinum_versioned, test_custom)
{
    ASSERT_EQ(ut_custom_application::trinum_version(), (core::trinum_version<1, 2, 4>{}));
    ASSERT_EQ(ut_custom_application::trinum_version().major(), 1);
    ASSERT_EQ(ut_custom_application::trinum_version().minor(), 2);
    ASSERT_EQ(ut_custom_application::trinum_version().patch(), 4);
}
