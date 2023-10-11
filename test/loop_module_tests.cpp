#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/application/module/module.hpp>
#include <arba/appt/application/module/decorator/loop.hpp>
#include <gtest/gtest.h>
#include <cstdlib>

using namespace std::string_literals;

class ut_application : public appt::adec::multi_task<appt::application<>, ut_application>
{
public:
    using appt::adec::multi_task<appt::application<>, ut_application>::multi_task;
};

class ut_times_up_module : public appt::module<ut_application>
{
public:
    virtual ~ut_times_up_module() override = default;

    virtual void run() override
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        app().stop_side_modules();
    }
};

class ut_loop_module : public appt::mdec::loop<appt::module<ut_application>, ut_loop_module>
{
private:
    using base_ = appt::mdec::loop<appt::module<ut_application>, ut_loop_module>;

public:
    ut_loop_module() : base_("ut_loop_module") {}
    virtual ~ut_loop_module() override = default;

    virtual void init() override
    {
        ++init_count;
    }

    void run_loop(appt::seconds delta_time)
    {
        ++run_count;
        if (run_count >= 10)
            stop();
    }

    virtual void finish() override
    {
        ++finish_count;
    }

    uint16_t init_count = 0;
    uint16_t run_count = 0;
    uint16_t finish_count = 0;
};

TEST(loop_module_tests, test_side_modules)
{
    ut_application app;
    ut_times_up_module& times_up_module = app.create_main_module<ut_times_up_module>();
    ASSERT_EQ(times_up_module.name(), "module_0");
    ut_loop_module& loop_module = app.create_module<ut_loop_module>();
    ASSERT_EQ(loop_module.name(), "ut_loop_module");
    loop_module.set_frequency(6);
    app.init();
    app.run();
    ASSERT_EQ(loop_module.init_count, 1);
    ASSERT_GE(loop_module.run_count, 6);
    ASSERT_LT(loop_module.run_count, 10);
    ASSERT_EQ(loop_module.finish_count, 1);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
