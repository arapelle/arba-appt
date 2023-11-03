#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/application/application.hpp>
#include <gtest/gtest.h>
#include <cstdlib>
#include "modules/counting_module.hpp"

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

using counting_module = ut::counting_module<ut_application>;

//-------------------
// unit tests
//-------------------

TEST(multi_task_application_mm_tests, test_main_module)
{
    ut_application app;
    counting_module& module = app.set_main_module(std::make_unique<counting_module>(app));
    ASSERT_EQ(module.name(), "module_0");
    ASSERT_EQ(app.init(), appt::execution_success);
    ASSERT_EQ(app.run(), appt::execution_success);
    ASSERT_EQ(module.run_count, 1);
    ASSERT_EQ(module.init_count, 1);
    counting_module& module_2 = app.create_main_module<counting_module>();
    ASSERT_EQ(module_2.name(), "module_1");
    app.init();
    std::ignore = app.run();
    ASSERT_EQ(module_2.run_count, 1);
    ASSERT_EQ(module_2.init_count, 1);
    counting_module& module_3 = app.create_main_module<counting_module>("main_module");
    ASSERT_EQ(module_3.name(), "main_module");
    app.init();
    std::ignore = app.run();
    ASSERT_EQ(module_3.run_count, 1);
    ASSERT_EQ(module_3.init_count, 1);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
