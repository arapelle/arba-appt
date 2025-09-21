#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/standard/application/application.hpp>

#include <arba/core/sbrm/sbrm.hpp>
#include <arba-appt-standard-ut/modules/counting_module.hpp>
#include <gtest/gtest.h>

#include <cstdlib>

using namespace std::string_literals;

//------------
// application
//------------

const std::filesystem::path program_dir = std::filesystem::temp_directory_path() / "root/dir";
std::array s_args = { (program_dir / "program_name.v2.run").generic_string(), "6"s, "-c"s, "Debug"s };
std::array cs_args = { s_args[0].data(), s_args[1].data(), s_args[2].data(), s_args[3].data() };
int pargc = cs_args.size();
char** pargv = cs_args.data();

class ut_application : public appt::adec::multi_task<appt::application<>, ut_application>
{
public:
    using appt::adec::multi_task<appt::application<>, ut_application>::multi_task;
};

TEST(multi_task_application_sm_tests, test_constructor_empty)
{
    ut_application app;
    ASSERT_TRUE(app.args().empty());
}

TEST(multi_task_application_sm_tests, test_constructor)
{
    ut_application app(core::program_args(pargc, pargv));
    ASSERT_EQ(app.args().argc, pargc);
    ASSERT_EQ(app.args().argv, pargv);
}

//-------------------
// module
//-------------------

using counting_module = ut::counting_module<ut_application>;

//-------------------
// unit tests
//-------------------

TEST(multi_task_application_sm_tests, test_side_modules)
{
    ut_application app(core::program_args(pargc, pargv));
    counting_module& module = app.add_module(std::make_unique<counting_module>(std::ref(app)));
    counting_module& module_2 = app.create_module<counting_module>("counting_module_2");
    counting_module& module_3 = app.create_module<counting_module>();
    ASSERT_EQ(module.name(), "module_0");
    ASSERT_EQ(module_2.name(), "counting_module_2");
    ASSERT_EQ(module_3.name(), "module_1");
    ASSERT_EQ(app.init(), appt::execution_statuses::success);
    ASSERT_EQ(app.run(), appt::execution_statuses::success);
    ASSERT_EQ(app.init(), appt::execution_statuses::success);
    ASSERT_EQ(app.run(), appt::execution_statuses::success);
    ASSERT_EQ(module.run_count, 2);
    ASSERT_EQ(module_2.run_count, 2);
    ASSERT_EQ(module_3.run_count, 2);
    ASSERT_EQ(module.init_count, 2);
    ASSERT_EQ(module_2.init_count, 2);
    ASSERT_EQ(module_3.init_count, 2);
}

int main(int argc, char** argv)
{
    std::filesystem::create_directories(program_dir);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
