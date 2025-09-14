#include <arba/appt/user/user_manager.hpp>

#include <gtest/gtest.h>

#include <cstdlib>

class ut_user : public appt::user
{
public:
    virtual ~ut_user() = default;

    ut_user(const std::string& input_name = "") : name(input_name) {}

    std::string name;
};

using ut_user_sptr = std::shared_ptr<ut_user>;

TEST(user_manager_tests, test_create_user)
{
    appt::user_manager<ut_user> users;
    ut_user_sptr anonyme_user = users.create_user();
    ut_user_sptr zeus_user = users.create_user("Zeus");
    ASSERT_EQ(anonyme_user->id(), 0);
    ASSERT_EQ(zeus_user->id(), 1);
    ASSERT_EQ(anonyme_user->name, "");
    ASSERT_EQ(zeus_user->name, "Zeus");
    ASSERT_EQ(users.size(), 2);
    ASSERT_EQ(users.max_number_of_users(), std::numeric_limits<std::size_t>::max());
}

TEST(user_manager_tests, test_max_number_of_users)
{
    appt::user_manager<ut_user> users;
    users.set_max_number_of_users(2);
    ASSERT_EQ(users.max_number_of_users(), 2);
    ut_user_sptr athena_user = users.create_user("Athena");
    ut_user_sptr zeus_user = users.create_user("Zeus");
    ASSERT_EQ(athena_user->id(), 0);
    ASSERT_EQ(zeus_user->id(), 1);
    ASSERT_EQ(athena_user->name, "Athena");
    ASSERT_EQ(zeus_user->name, "Zeus");
    ASSERT_EQ(users.size(), 2);
    ut_user_sptr null_user = users.create_user("Hades");
    ASSERT_EQ(null_user, nullptr);
    ASSERT_EQ(users.size(), 2);
}

TEST(user_manager_tests, test_release_user_id)
{
    appt::user_manager<ut_user> users;
    ut_user_sptr anonyme_user = users.create_user();
    ut_user_sptr zeus_user = users.create_user("Zeus");
    ut_user::id_type zeus_id = zeus_user->id();
    zeus_user.reset();
    users.release_user(zeus_id);
    ut_user_sptr athena_user = users.create_user("Athena");
    ut_user::id_type athena_id = athena_user->id();
    ASSERT_EQ(zeus_id, athena_id);
    ASSERT_EQ(users.size(), 2);
}

TEST(user_manager_tests, test_release_user)
{
    appt::user_manager<ut_user> users;
    users.create_user();
    ut_user_sptr zeus_user = users.create_user("Zeus");
    ut_user::id_type zeus_id = zeus_user->id();
    users.reset_user_shared_ptr(zeus_user);
    ut_user_sptr athena_user = users.create_user("Athena");
    ut_user::id_type athena_id = athena_user->id();
    ASSERT_EQ(zeus_id, athena_id);
    ASSERT_EQ(athena_user->name, "Athena");
}

TEST(user_manager_tests, test_release_user_2)
{
    appt::user_manager<ut_user> users;
    users.create_user("Hades");
    ut_user_sptr zeus_user = users.create_user("Zeus");
    users.create_user("Heracles");
    ut_user::id_type zeus_id = zeus_user->id();
    users.reset_user_shared_ptr(zeus_user);
    ut_user_sptr athena_user = users.create_user("Athena");
    ut_user::id_type athena_id = athena_user->id();
    ASSERT_EQ(zeus_id, athena_id);
    ASSERT_EQ(athena_user->name, "Athena");
}

TEST(user_manager_tests, test_shared_user)
{
    appt::user_manager<ut_user> users;
    ut_user_sptr zeus_user = users.create_user("Zeus");
    ut_user::id_type zeus_id = zeus_user->id();
    ASSERT_EQ(users.shared_user(zeus_id), zeus_user);
    users.reset_user_shared_ptr(zeus_user);
    ASSERT_EQ(users.shared_user(zeus_id), nullptr);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
