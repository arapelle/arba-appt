#include <appt/multi_user_application.hpp>
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

class ut_user : public appt::user
{
public:
    virtual ~ut_user() = default;

    ut_user(const std::string& name = "") : name(name) {}

    std::string name;
};

using ut_user_sptr = std::shared_ptr<ut_user>;

class ut_application : public appt::multi_user_application<ut_user, ut_application>
{
private:
    using base_ = appt::multi_user_application<ut_user, ut_application>;

public:
    using base_::multi_user;

    void run()
    {
        std::vector<std::string> names{ "Alpha", "Beta", "Gamma" };
        for (const auto& name : names)
            users.push_back(usr_manager().create_user(name));
    }

    std::vector<std::shared_ptr<ut_user>> users;
};

TEST(multi_user_application_tests, test_constructor_empty)
{
    ut_application app;
    ASSERT_TRUE(app.args().empty());
}

TEST(multi_user_application_tests, test_constructor)
{
    ut_application app(argc, argv);
    ASSERT_EQ(app.args().argc, argc);
    ASSERT_EQ(app.args().argv, argv);
}

TEST(multi_user_application_tests, test_run)
{
    ut_application app(argc, argv);
    app.run();
    ASSERT_EQ(app.usr_manager().size(), 3);
    std::shared_ptr user_sptr = app.usr_manager().shared_user(0);
    ASSERT_NE(user_sptr, nullptr);
    ASSERT_EQ(user_sptr->name, "Alpha");
    user_sptr = app.usr_manager().shared_user(1);
    ASSERT_EQ(user_sptr->name, "Beta");
    user_sptr = app.usr_manager().shared_user(2);
    ASSERT_EQ(user_sptr->name, "Gamma");
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
