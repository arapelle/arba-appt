#include <arba/appt/application/decorator/multi_task.hpp>
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

class ut_application : public appt::adec::multi_task<appt::application, ut_application>
{
public:
    using appt::adec::multi_task<appt::application, ut_application>::multi_task;
};

TEST(multi_task_application_tests, test_constructor_empty)
{
    ut_application app;
    ASSERT_TRUE(app.args().empty());
}

TEST(multi_task_application_tests, test_constructor)
{
    ut_application app(argc, argv);
    ASSERT_EQ(app.args().argc, argc);
    ASSERT_EQ(app.args().argv, argv);
}

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

TEST(multi_task_application_tests, test_side_modules)
{
    ut_application app(argc, argv);
    run_count_module& module = app.add_module(std::make_unique<run_count_module>());
    run_count_module& module_2 = app.create_module<run_count_module>("run_count_module_2");
    run_count_module& module_3 = app.create_module<run_count_module>();
    ASSERT_EQ(module.name(), "module_0");
    ASSERT_EQ(module_2.name(), "run_count_module_2");
    ASSERT_EQ(module_3.name(), "module_1");
    app.init();
    app.run();
    app.init();
    app.run();
    ASSERT_EQ(module.run_count, 2);
    ASSERT_EQ(module_2.run_count, 2);
    ASSERT_EQ(module_3.run_count, 2);
    ASSERT_EQ(module.init_count, 2);
    ASSERT_EQ(module_2.init_count, 2);
    ASSERT_EQ(module_3.init_count, 2);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
