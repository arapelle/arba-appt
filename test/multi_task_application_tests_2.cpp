#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/application/application.hpp>
#include <gtest/gtest.h>
#include <cstdlib>

using namespace std::string_literals;

//------------
// application
//------------

class ut_application : public appt::adec::multi_task<appt::application<>, ut_application>
{
public:
    using appt::adec::multi_task<appt::application<>, ut_application>::multi_task;
};

//-------------------
// module
//-------------------

class run_count_module : public appt::module<ut_application>
{
public:
    run_count_module(std::string_view module_name = std::string_view())
        : appt::module<ut_application>(module_name)
    {}

    virtual ~run_count_module() override = default;

    virtual void init() override
    {
        ++init_count;
    }

    virtual void run() override
    {
        ++run_count;
    }

    uint16_t run_count = 0;
    uint16_t init_count = 0;
};

TEST(multi_task_application_tests_2, test_main_module)
{
    ut_application app;
    run_count_module& module = app.set_main_module(std::make_unique<run_count_module>());
    ASSERT_EQ(module.name(), "module_0");
    app.init();
    app.run();
    ASSERT_EQ(module.run_count, 1);
    ASSERT_EQ(module.init_count, 1);
    run_count_module& module_2 = app.create_main_module<run_count_module>();
    ASSERT_EQ(module_2.name(), "module_1");
    app.init();
    app.run();
    ASSERT_EQ(module_2.run_count, 1);
    ASSERT_EQ(module_2.init_count, 1);
    run_count_module& module_3 = app.create_main_module<run_count_module>("main_module");
    ASSERT_EQ(module_3.name(), "main_module");
    app.init();
    app.run();
    ASSERT_EQ(module_3.run_count, 1);
    ASSERT_EQ(module_3.init_count, 1);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
