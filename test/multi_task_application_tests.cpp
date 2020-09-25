#include <appt/multi_task_application.hpp>
#include <gtest/gtest.h>
#include <cstdlib>

using namespace std::string_literals;

//------------
// application
//------------

std::array s_args = { "/root/dir/program_name.v2.run"s, "6"s, "-c"s, "Debug"s };
std::array cs_args = { s_args[0].data(), s_args[1].data(), s_args[2].data(), s_args[3].data() };
int argc = cs_args.size();
char** argv = cs_args.data();

TEST(multi_task_application_tests, test_constructor_empty)
{
    appt::multi_task_application app;
    ASSERT_TRUE(app.args().empty());
}

TEST(multi_task_application_tests, test_constructor)
{
    appt::multi_task_application app(argc, argv);
    ASSERT_EQ(app.args().argc, argc);
    ASSERT_EQ(app.args().argv, argv);
}

//-------------------
// application_module
//-------------------

class ut_application : public appt::multi_task_application
{
public:
    using appt::multi_task_application::multi_task_application;
};

class run_count_module : public appt::application_module<ut_application>
{
public:
    using appt::application_module<ut_application>::application_module;
    virtual ~run_count_module() override = default;

    virtual void run() override
    {
        ++run_count;
    }

    uint8_t run_count = 0;
};

TEST(multi_task_application_tests, test_module)
{
    ut_application app(argc, argv);
    run_count_module& module = app.add_module(std::make_unique<run_count_module>(app));
    app.init();
    app.run();
    app.run();
    ASSERT_EQ(module.run_count, 2);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
