#include <appt/multi_task_application.hpp>
#include <appt/loop_module.hpp>
#include <gtest/gtest.h>
#include <cstdlib>

using namespace std::string_literals;

class ut_application : public appt::multi_task_application<ut_application>
{
public:
    using appt::multi_task_application<ut_application>::multi_task;
};

class ut_times_up_module : public appt::module<ut_application>
{
public:
    virtual ~ut_times_up_module() override = default;

    virtual void run() override
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        app().stop_side_modules();
        std::cout << "TIME IS UP" << std::endl;
    }
};

class ut_loop_module : public appt::loop_module<ut_loop_module, ut_application>
{
public:
    virtual ~ut_loop_module() override = default;

    virtual void init() override
    {
    }

    void run_loop(appt::seconds delta_time)
    {
        ++run_count;
        std::cout << delta_time.to_milliseconds() << "     " << delta_time.to_seconds() << "     " << delta_time.count() << std::endl;
    }

    virtual void finish() override
    {
        std::cout << "run_loop_count: " << run_count << std::endl;
    }

    uint16_t run_count = 0;
};

TEST(multi_task_application_tests, test_side_modules)
{
    ut_application app;
    app.create_main_module<ut_times_up_module>();
    ut_loop_module& loop_module = app.create_module<ut_loop_module>();
    loop_module.set_frequency(60);
    app.init();
    app.run();
    ASSERT_GE(loop_module.run_count, 60);
    ASSERT_LT(loop_module.run_count, 65);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
