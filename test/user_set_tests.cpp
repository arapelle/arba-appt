#include <appt/user/user_set.hpp>
#include <gtest/gtest.h>
#include <unordered_set>
#include <cstdlib>

using namespace std::string_literals;

//------------
// application
//------------

class ut_user : public appt::user
{
public:
    virtual ~ut_user() = default;

    ut_user(const std::string& name = "") : name_(name) {}
    const std::string& name() const { return name_; }

public:
    std::string name_;
};

using ut_user_sptr = std::shared_ptr<ut_user>;

class ut_user_2 : public appt::user
{
public:
    virtual ~ut_user_2() = default;

    ut_user_2(const std::string& name = "") : name(name) {}

    std::string name;
};

using ut_user_2_sptr = std::shared_ptr<ut_user_2>;

//--------------------------------------------------------------------------------

TEST(user_set_tests, test_user_set_name)
{
    using sptr_hash = appt::user_sptr_name_hash<ut_user>;

    appt::user_set<ut_user, sptr_hash> ut_user_set;
    std::string user_name("Zeus");
    std::shared_ptr user_sptr = std::make_shared<ut_user>(user_name);
    ut_user_set.insert_user(user_sptr);
    ut_user_set.insert_user(std::make_shared<ut_user>("Athena"));
    auto iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter != ut_user_set.end());
    ASSERT_EQ((*iter)->name(), user_name);
    ut_user_set.erase_user(user_name);
    iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter == ut_user_set.end());
}

TEST(user_set_tests, test_user_set_name_2)
{
    using sptr_hash = appt::user_sptr_name_hash<ut_user_2>;

    appt::user_set<ut_user_2, sptr_hash> ut_user_set;
    std::string user_name("Zeus");
    std::shared_ptr user_sptr = std::make_shared<ut_user_2>(user_name);
    ut_user_set.insert_user(user_sptr);
    ut_user_set.insert_user(std::make_shared<ut_user_2>("Athena"));
    auto iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter != ut_user_set.end());
    ASSERT_EQ((*iter)->name, user_name);
    ut_user_set.erase_user(user_name);
    iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter == ut_user_set.end());
}

TEST(user_set_tests, test_user_set_mem_fn)
{
    using sptr_hash = appt::user_sptr_mem_fn_hash<ut_user, std::string, &ut_user::name>;

    appt::user_set<ut_user, sptr_hash> ut_user_set;
    std::string user_name("Zeus");
    std::shared_ptr user_sptr = std::make_shared<ut_user>(user_name);
    ut_user_set.insert_user(user_sptr);
    ut_user_set.insert_user(std::make_shared<ut_user>("Athena"));
    auto iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter != ut_user_set.end());
    ASSERT_EQ((*iter)->name(), user_name);
    ut_user_set.erase_user(user_name);
    iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter == ut_user_set.end());
}

TEST(user_set_tests, test_user_set_mem)
{
    using sptr_hash = appt::user_sptr_mem_hash<ut_user_2, std::string, &ut_user_2::name>;

    appt::user_set<ut_user_2, sptr_hash> ut_user_set;
    std::string user_name("Zeus");
    std::shared_ptr user_sptr = std::make_shared<ut_user_2>(user_name);
    ut_user_set.insert_user(user_sptr);
    ut_user_set.insert_user(std::make_shared<ut_user_2>("Athena"));
    auto iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter != ut_user_set.end());
    ASSERT_EQ((*iter)->name, user_name);
    ut_user_set.erase_user(user_name);
    iter = ut_user_set.find_user(user_name);
    ASSERT_TRUE(iter == ut_user_set.end());
}

TEST(user_set_tests, test_user_set_max_number_of_users)
{
    using sptr_hash = appt::user_sptr_name_hash<ut_user>;

    appt::user_set<ut_user, sptr_hash> ut_user_set;
    ut_user_set.set_max_number_of_users(3);
    ut_user_sptr alpha_user = ut_user_set.create_user("Alpha");
    ut_user_sptr beta_user = ut_user_set.create_user("Beta");
    ut_user_sptr lambda_user = ut_user_set.create_user("Lambda");
    ut_user_sptr omega_user = ut_user_set.create_user("Omega");
    ASSERT_NE(alpha_user, nullptr);
    ASSERT_NE(beta_user, nullptr);
    ASSERT_NE(lambda_user, nullptr);
    ASSERT_EQ(omega_user, nullptr);

    ASSERT_EQ(ut_user_set.size(), 3);
}

TEST(user_set_tests, test_user_set_max_number_of_users_2)
{
    using sptr_hash = appt::user_sptr_name_hash<ut_user>;

    appt::user_set<ut_user, sptr_hash> ut_user_set;
    ut_user_set.set_max_number_of_users(3);
    ut_user_set.insert_user(std::make_shared<ut_user>("Alpha"));
    ut_user_set.insert_user(std::make_shared<ut_user>("Beta"));
    ut_user_set.insert_user(std::make_shared<ut_user>("Lambda"));
    auto res = ut_user_set.insert_user(std::make_shared<ut_user>("Omega"));
    ASSERT_EQ(res.first, ut_user_set.end());
    ASSERT_FALSE(res.second);

    ASSERT_EQ(ut_user_set.size(), 3);
}

TEST(user_set_tests, test_user_set_id_with_user_manager)
{
    using sptr_hash = appt::user_sptr_id_hash<ut_user>;

    appt::user_manager<ut_user> user_manager;

    appt::user_set<ut_user, sptr_hash> ut_user_set;
    ut_user_set.set_user_manager(user_manager);

    ut_user_sptr alpha_user = ut_user_set.create_user("Alpha");
    appt::user_id alpha_id = alpha_user->id();
    ut_user_set.create_user("Beta");

    auto iter = ut_user_set.find_user(alpha_id);
    ASSERT_TRUE(iter != ut_user_set.end());
    ASSERT_EQ((*iter)->id(), alpha_id);
    ASSERT_EQ((*iter)->name(), "Alpha");
    ASSERT_EQ(user_manager.shared_user(alpha_id), alpha_user);
    ASSERT_EQ(ut_user_set.size(), 2);

    alpha_user.reset();
    ut_user_set.erase_user(alpha_id);
    iter = ut_user_set.find_user(alpha_id);
    ASSERT_TRUE(iter == ut_user_set.end());
    ASSERT_EQ(user_manager.shared_user(alpha_id), nullptr);
}

TEST(user_set_tests, test_user_set_create_twice_with_user_manager)
{
    using sptr_hash = appt::user_sptr_name_hash<ut_user>;

    appt::user_manager<ut_user> user_manager;

    appt::user_set<ut_user, sptr_hash> ut_user_set;
    ut_user_set.set_user_manager(user_manager);

    ut_user_sptr alpha_user = ut_user_set.create_user("Alpha");
    ut_user_set.create_user("Beta");
    ASSERT_EQ(user_manager.size(), 2);

    ASSERT_EQ(*ut_user_set.find_user(alpha_user->name()), alpha_user);
    ut_user_sptr alpha_user_bis = ut_user_set.create_user("Alpha");
    ASSERT_EQ(ut_user_set.size(), 2);
    ASSERT_EQ(alpha_user_bis, nullptr);
    ASSERT_EQ(user_manager.shared_user(alpha_user->id()), alpha_user);
    ASSERT_EQ(user_manager.size(), 2);
}

TEST(user_set_tests, test_user_set_id_with_user_manager_with_max_user_limit)
{
    using sptr_hash = appt::user_sptr_id_hash<ut_user>;

    appt::user_manager<ut_user> user_manager;
    user_manager.set_max_number_of_users(3);

    appt::user_set<ut_user, sptr_hash> ut_user_set;
    ut_user_set.set_user_manager(user_manager);
    ut_user_sptr alpha_user = ut_user_set.create_user("Alpha");
    ut_user_sptr beta_user = ut_user_set.create_user("Beta");
    ut_user_sptr lambda_user = ut_user_set.create_user("Lambda");
    ut_user_sptr omega_user = ut_user_set.create_user("Omega");
    ASSERT_EQ(omega_user, nullptr);

    ASSERT_EQ(user_manager.size(), 3);
    ASSERT_EQ(ut_user_set.size(), 3);
}

TEST(user_set_tests, test_clear_users)
{
    using sptr_hash = appt::user_sptr_id_hash<ut_user>;

    appt::user_manager<ut_user> user_manager;

    appt::user_set<ut_user, sptr_hash> ut_user_set;
    ut_user_set.set_user_manager(user_manager);

    ut_user_set.create_user("Elza");
    ASSERT_NE(ut_user_set.find_user(0), ut_user_set.end());
    ut_user_set.create_user("Era");
    ut_user_set.create_user("Emma");
    ASSERT_EQ(ut_user_set.size(), 3);
    ASSERT_EQ(user_manager.size(), 3);

    ut_user_set.clear_users();
    ASSERT_EQ(ut_user_set.find_user(0), ut_user_set.end());
    ASSERT_EQ(ut_user_set.size(), 0);
    ASSERT_EQ(user_manager.size(), 0);
}

TEST(user_set_tests, test_destructor)
{
    using sptr_hash = appt::user_sptr_id_hash<ut_user>;

    appt::user_manager<ut_user> user_manager;

    {
        appt::user_set<ut_user, sptr_hash> ut_user_set;
        ut_user_set.set_user_manager(user_manager);
        ut_user_set.create_user("Elza");
        ut_user_set.create_user("Era");
        ut_user_set.create_user("Emma");
        ASSERT_EQ(user_manager.size(), 3);
    }

    ASSERT_EQ(user_manager.size(), 0);
}

TEST(user_set_tests, test_copy)
{
    using sptr_hash = appt::user_sptr_id_hash<ut_user>;

    appt::user_manager<ut_user> user_manager;

    {
        appt::user_set<ut_user, sptr_hash> ut_user_set;
        ut_user_set.set_user_manager(user_manager);
        ut_user_set.create_user("Elza");
        ut_user_set.create_user("Era");
        ut_user_set.create_user("Emma");
        ASSERT_EQ(user_manager.size(), 3);

        appt::user_set<ut_user, sptr_hash> ut_user_set_2(ut_user_set);
        ASSERT_EQ(ut_user_set.size(), ut_user_set_2.size());
        ASSERT_EQ(ut_user_set.find_user_sptr(0), ut_user_set_2.find_user_sptr(0));
        ASSERT_EQ(ut_user_set.find_user_sptr(1), ut_user_set_2.find_user_sptr(1));
        ASSERT_EQ(ut_user_set.find_user_sptr(2), ut_user_set_2.find_user_sptr(2));
        ASSERT_EQ(user_manager.size(), 3);
    }

    ASSERT_EQ(user_manager.size(), 0);
}

TEST(user_set_tests, test_assignment)
{
    using sptr_hash = appt::user_sptr_id_hash<ut_user>;

    appt::user_manager<ut_user> user_manager;

    {
        appt::user_set<ut_user, sptr_hash> ut_user_set;
        ut_user_set.set_user_manager(user_manager);
        ut_user_set.create_user("Elza");
        ut_user_set.create_user("Era");
        ut_user_set.create_user("Emma");
        ASSERT_EQ(user_manager.size(), 3);

        appt::user_set<ut_user, sptr_hash> ut_user_set_2;
        ut_user_set_2 = ut_user_set;
        ASSERT_EQ(ut_user_set.size(), ut_user_set_2.size());
        ASSERT_EQ(ut_user_set.find_user_sptr(0), ut_user_set_2.find_user_sptr(0));
        ASSERT_EQ(ut_user_set.find_user_sptr(1), ut_user_set_2.find_user_sptr(1));
        ASSERT_EQ(ut_user_set.find_user_sptr(2), ut_user_set_2.find_user_sptr(2));
        ASSERT_EQ(user_manager.size(), 3);
    }

    ASSERT_EQ(user_manager.size(), 0);
}

TEST(user_set_tests, test_move_assignment)
{
    using sptr_hash = appt::user_sptr_id_hash<ut_user>;

    appt::user_manager<ut_user> user_manager;

    {
        appt::user_set<ut_user, sptr_hash> ut_user_set;
        ut_user_set.set_user_manager(user_manager);
        ut_user_set.create_user("Elza");
        ut_user_set.create_user("Era");
        ut_user_set.create_user("Emma");
        ASSERT_EQ(user_manager.size(), 3);

        appt::user_set<ut_user, sptr_hash> ut_user_set_2;
        ut_user_set_2 = std::move(ut_user_set);
        ASSERT_EQ(ut_user_set.size(), 0);
        ASSERT_EQ(ut_user_set_2.size(), 3);
        ASSERT_EQ(ut_user_set_2.find_user_sptr(0)->name(), "Elza");
        ASSERT_EQ(ut_user_set_2.find_user_sptr(1)->name(), "Era");
        ASSERT_EQ(ut_user_set_2.find_user_sptr(2)->name(), "Emma");
        ASSERT_EQ(user_manager.size(), 3);
    }

    ASSERT_EQ(user_manager.size(), 0);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
