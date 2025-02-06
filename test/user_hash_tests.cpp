#include <arba/appt/user/user_hash.hpp>
#include <arba/appt/user/user_manager.hpp>

#include <gtest/gtest.h>

#include <cstdlib>
#include <unordered_set>

using namespace std::string_literals;

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

TEST(user_hash_tests, test_user_sptr_id_hash)
{
    using sptr_hash = appt::user_sptr_id_hash<ut_user>;

    appt::user_manager<ut_user> user_manager;
    std::shared_ptr user_sptr = user_manager.create_user("Namae");

    sptr_hash s_hash;
    ASSERT_EQ(s_hash.user_id(*user_sptr), 0);
    ASSERT_EQ(s_hash(user_sptr), 0);
    ASSERT_EQ(s_hash(nullptr), std::numeric_limits<std::size_t>::max());
}

TEST(user_hash_tests, test_user_sptr_name_hash)
{
    using sptr_hash = appt::user_sptr_name_hash<ut_user>;

    std::shared_ptr user_sptr = std::make_shared<ut_user>("Namae");

    sptr_hash s_hash;
    ASSERT_EQ(s_hash.user_id(*user_sptr), "Namae");
    ASSERT_EQ(s_hash(user_sptr), std::hash<std::string_view>{}("Namae"));
    ASSERT_EQ(s_hash("Namae"), std::hash<std::string_view>{}("Namae"));
}

TEST(user_hash_tests, test_user_2_sptr_name_hash)
{
    using sptr_hash = appt::user_sptr_name_hash<ut_user_2>;

    std::shared_ptr user_sptr = std::make_shared<ut_user_2>("Namae");

    sptr_hash s_hash;
    ASSERT_EQ(s_hash.user_id(*user_sptr), "Namae");
    ASSERT_EQ(s_hash(user_sptr), std::hash<std::string_view>{}("Namae"));
    ASSERT_EQ(s_hash("Namae"), std::hash<std::string_view>{}("Namae"));
}

TEST(user_hash_tests, test_user_sptr_mem_fn_hash)
{
    using sptr_hash = appt::user_sptr_mem_fn_hash<ut_user, std::string, &ut_user::name>;

    std::shared_ptr user_sptr = std::make_shared<ut_user>("Namae");

    sptr_hash s_hash;
    ASSERT_EQ(s_hash.user_id(*user_sptr), "Namae");
    ASSERT_EQ(s_hash(user_sptr), std::hash<std::string_view>{}("Namae"));
    ASSERT_EQ(s_hash("Namae"), std::hash<std::string_view>{}("Namae"));
}

TEST(user_hash_tests, test_user_sptr_mem_hash)
{
    using sptr_hash = appt::user_sptr_mem_hash<ut_user_2, std::string, &ut_user_2::name>;

    std::shared_ptr user_sptr = std::make_shared<ut_user_2>("Namae");

    sptr_hash s_hash;
    ASSERT_EQ(s_hash.user_id(*user_sptr), "Namae");
    ASSERT_EQ(s_hash(user_sptr), std::hash<std::string_view>{}("Namae"));
    ASSERT_EQ(s_hash("Namae"), std::hash<std::string_view>{}("Namae"));
}

// test unordered_set with custom hash:

TEST(user_hash_tests, test_unordered_set_user)
{
    std::unordered_set<ut_user, std::hash<appt::user>> ut_user_set;
    ut_user_set.emplace("Eole");
}

TEST(user_hash_tests, test_unordered_set_user_sptr)
{
    std::unordered_set<ut_user_sptr> ut_user_set;
    ut_user_set.insert(std::make_shared<ut_user>("Eole"));
}

TEST(user_hash_tests, test_unordered_set_user_sptr_id)
{
    std::unordered_set<ut_user_sptr, appt::user_sptr_id_hash<ut_user>> ut_user_set;
    ut_user_set.insert(std::make_shared<ut_user>("Eole"));
}

TEST(user_hash_tests, test_unordered_set_user_sptr_name)
{
    std::unordered_set<ut_user_sptr, appt::user_sptr_name_hash<ut_user>> ut_user_set;
    std::string user_name("Zeus");
    std::shared_ptr user_sptr = std::make_shared<ut_user>(user_name);
    ut_user_set.insert(user_sptr);
    ut_user_set.insert(std::make_shared<ut_user>("Athena"));
    auto iter = ut_user_set.find(user_sptr);
    ASSERT_TRUE(iter != ut_user_set.end());
    ASSERT_EQ((*iter)->name(), user_name);
    //    ut_user_set.find(user_name);
}

TEST(user_hash_tests, test_unordered_set_user_2_sptr_name)
{
    std::unordered_set<ut_user_2_sptr, appt::user_sptr_name_hash<ut_user_2>> ut_user_set;
    std::string user_name("Zeus");
    std::shared_ptr user_sptr = std::make_shared<ut_user_2>(user_name);
    ut_user_set.insert(user_sptr);
    ut_user_set.insert(std::make_shared<ut_user_2>("Athena"));
    auto iter = ut_user_set.find(user_sptr);
    ASSERT_TRUE(iter != ut_user_set.end());
    ASSERT_EQ((*iter)->name, user_name);
    //    ut_user_set.find(user_name);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
