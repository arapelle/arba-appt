#include <arba/appt/application/application.hpp>

#include <gtest/gtest.h>

#include <cstdlib>

using namespace std::string_literals;

const std::filesystem::path program_dir = std::filesystem::temp_directory_path() / "root/dir";
std::array s_args = { (program_dir / "program_name.v2.run").generic_string(), "6"s, "-c"s, "Debug"s };
std::array cs_args = { s_args[0].data(), s_args[1].data(), s_args[2].data(), s_args[3].data() };
int pargc = cs_args.size();
char** pargv = cs_args.data();

TEST(application_tests, test_constructor_empty)
{
    arba::appt::application<> app;
    ASSERT_TRUE(app.args().empty());
}

TEST(application_tests, test_constructor)
{
    appt::application<> app(core::program_args(pargc, pargv));
    ASSERT_EQ(app.args().argc, pargc);
    ASSERT_EQ(app.args().argv, pargv);
}

int main(int argc, char** argv)
{
    std::filesystem::create_directories(program_dir);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
