#include <appt/application.hpp>
#include <gtest/gtest.h>
#include <cstdlib>

using namespace std::string_literals;

std::array s_args = { "/root/dir/program_name.v2.run"s, "6"s, "-c"s, "Debug"s };
std::array cs_args = { s_args[0].data(), s_args[1].data(), s_args[2].data(), s_args[3].data() };
int argc = cs_args.size();
char** argv = cs_args.data();

TEST(application_tests, test_constructor_empty)
{
    appt::application app;
    ASSERT_TRUE(app.args().empty());
}

TEST(application_tests, test_constructor)
{
    appt::application app(argc, argv);
    ASSERT_EQ(app.args().argc, argc);
    ASSERT_EQ(app.args().argv, argv);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
