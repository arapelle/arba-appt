#include <arba/appt/application/application.hpp>
#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/application/decorator/multi_user.hpp>
#include <arba/appt/application/module/module.hpp>
#include <arba/appt/application/module/decorator/multi_user.hpp>
#include <gtest/gtest.h>
#include <cstdlib>

using namespace std::string_literals;

//------------
// application
//------------

const std::filesystem::path program_dir = std::filesystem::temp_directory_path() / "root/dir";
std::array s_args = { (program_dir / "program_name.v2.run").generic_string(), "6"s, "-c"s, "Debug"s };
std::array cs_args = { s_args[0].data(), s_args[1].data(), s_args[2].data(), s_args[3].data() };
int argc = cs_args.size();
char** argv = cs_args.data();

class ut_user : public appt::user
{
public:
    virtual ~ut_user() = default;

    ut_user(const std::string& name = "") : name_(name) {}
    const std::string& name() const { return name_; }

public:
    std::string name_;
};

class ut_application : public appt::multi_task<appt::adec::multi_user<ut_user, appt::application>, ut_application>
{
    using base_ = appt::multi_task<appt::adec::multi_user<ut_user, appt::application>, ut_application>;

public:
    using base_::multi_task;
};

TEST(multi_user_multi_task_application_tests, test_constructor_empty)
{
    ut_application app;
    ASSERT_TRUE(app.args().empty());
}

TEST(multi_user_multi_task_application_tests, test_constructor)
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
    virtual ~run_count_module() override = default;

    virtual void init() override
    {
        ++init_count;
    }

    virtual void run() override
    {
        ++run_count;
    }

    uint16_t init_count = 0;
    uint16_t run_count = 0;
};

TEST(multi_user_multi_task_application_tests, test_side_modules)
{
    ut_application app(argc, argv);
    run_count_module& module = app.add_module(std::make_unique<run_count_module>());
    run_count_module& module_2 = app.create_module<run_count_module>();
    app.init();
    app.run();
    app.init();
    app.run();
    ASSERT_EQ(module.run_count, 2);
    ASSERT_EQ(module_2.run_count, 2);
    ASSERT_EQ(module.init_count, 2);
    ASSERT_EQ(module_2.init_count, 2);
}

TEST(multi_user_multi_task_application_tests, test_main_module)
{
    ut_application app(argc, argv);
    run_count_module& module = app.set_main_module(std::make_unique<run_count_module>());
    app.init();
    app.run();
    run_count_module& module_2 = app.create_main_module<run_count_module>();
    app.init();
    app.run();
    ASSERT_EQ(module.run_count, 1);
    ASSERT_EQ(module_2.run_count, 1);
    ASSERT_EQ(module.init_count, 1);
    ASSERT_EQ(module_2.init_count, 1);
}

class multi_user_module : public appt::mdec::multi_user<ut_user, appt::user_sptr_id_hash<ut_user>, appt::module<ut_application>>
{
public:
    virtual ~multi_user_module() override = default;

    virtual void init() override
    {
        users().clear_users();
        ASSERT_EQ(users().size(), 0);
    }

    virtual void run() override
    {
        for (const auto& name : names)
            users().create_user(name);
    }

    void print_users()
    {
        std::cout << "users:" << std::endl;
        for (const auto& user_sptr : users())
            std::cout << user_sptr->id() << ": " << user_sptr->name() << std::endl;
        std::cout << std::endl;
    }

    std::vector<std::string> names;
};

TEST(multi_user_multi_task_application_tests, test_create_multi_user_module)
{
    ut_application app(argc, argv);
    multi_user_module& module = app.create_module<multi_user_module>();

    module.names.push_back("Alpha");
    app.init();
    app.run();
    ASSERT_EQ(module.users().size(), 1);
    auto iter = module.users().find_user(0);
    ASSERT_EQ((*iter)->name(), "Alpha");

    module.names.push_back("Beta");
    module.names.push_back("Gamma");
    app.init();
    app.run();
    ASSERT_EQ(module.users().size(), 3);
    iter = module.users().find_user(0);
    ASSERT_EQ((*iter)->name(), "Alpha");
    iter = module.users().find_user(1);
    ASSERT_EQ((*iter)->name(), "Beta");
    iter = module.users().find_user(2);
    ASSERT_EQ((*iter)->name(), "Gamma");
}

int main(int argc, char** argv)
{
    std::filesystem::create_directories(program_dir);
    ::testing::InitGoogleTest(&argc, argv);
    auto res = RUN_ALL_TESTS();
    std::filesystem::remove_all(program_dir);
    return res;
}
