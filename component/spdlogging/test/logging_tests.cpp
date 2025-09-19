#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <arba/appt/application/basic_application.hpp>
#include <arba/appt/spdlogging/application/decorator/spdlogging.hpp>
#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/spdlogging/application/module/decorator/spdlogging.hpp>
#include <arba/appt/application/module/decorator/loop.hpp>
#include <arba/appt/application/module/basic_module.hpp>

#include <arba/stdx/chrono/format_time_point.hpp>
#include <gtest/gtest.h>

#include <cstdlib>
#include <fstream>

using namespace std::string_literals;

const std::filesystem::path program_dir = std::filesystem::temp_directory_path() / "root/dir";
std::array s_args = { (program_dir / "program_name.v2.run").generic_string(), "6"s, "-c"s, "Debug"s };
std::array cs_args = { s_args[0].data(), s_args[1].data(), s_args[2].data(), s_args[3].data() };
int pargc = cs_args.size();
char** pargv = cs_args.data();

namespace ut
{
class application_base
{
private:
    application_base() = delete;
    using logging_application_ = appt::adec::spdlogging<appt::basic_application<>>;
    using multi_task_application_ = appt::adec::multi_task<logging_application_>;

public:
    template <class app_type>
    using type = typename multi_task_application_::rebind_t<app_type>;
};

class application : public application_base::type<application>
{
private:
    using base_ = application_base::type<application>;

public:
    using base_::base_;

    inline std::filesystem::path make_log_directory_path() const
    {
        std::filesystem::path subpath = std::filesystem::path("log") / stdx::format_Ymd_HMS_as_filename();
        if (!args().empty())
            return args().program_dir() / subpath;
        return subpath;
    }

    void init()
    {
        base_::init();
        logger()->set_level(spdlog::level::trace);
    }
};

class times_up_module : public appt::basic_module<application, times_up_module>
{
    using base_ = appt::basic_module<application, times_up_module>;

public:
    using base_::base_;

    virtual ~times_up_module() override = default;

    virtual void run() override
    {
        SPDLOG_LOGGER_TRACE(app().logger(), "");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        app().stop_side_modules();
    }
};

class first_module : public appt::mdec::loop<appt::mdec::spdlogging<appt::basic_module<application>>, first_module>
{
private:
    using base_ = appt::mdec::loop<appt::mdec::spdlogging<appt::basic_module<application>>, first_module>;

public:
    first_module(application_type& app) : base_(app, "first_module") {}
    virtual ~first_module() override = default;

    void run_loop(appt::dt::seconds) { SPDLOG_LOGGER_INFO(logger(), ""); }

    virtual void finish() override { SPDLOG_LOGGER_INFO(logger(), ""); }
};

class second_module : public appt::mdec::loop<appt::mdec::spdlogging<appt::basic_module<application>>, second_module>
{
private:
    using base_ = appt::mdec::loop<appt::mdec::spdlogging<appt::basic_module<application>>, second_module>;

public:
    second_module(application_type& app) : base_(app, "second_module") {}
    virtual ~second_module() override = default;

    virtual void init() override
    {
        this->base_::init();
        logger()->set_level(spdlog::level::debug);
    }

    void run_loop(appt::dt::seconds) { SPDLOG_LOGGER_DEBUG(logger(), ""); }

    virtual void finish() override { SPDLOG_LOGGER_DEBUG(logger(), ""); }
};

} // namespace ut

TEST(spdlogging_tests, test_logs__no_args)
{
    {
        ut::application app;
        ASSERT_TRUE(std::filesystem::exists(app.log_dir()));
        ASSERT_NE(app.logger(), nullptr);
        std::filesystem::path app_log_file = app.log_dir() / "application.log";
        ASSERT_TRUE(std::filesystem::exists(app_log_file));
        ut::first_module& first_module = app.create_module<ut::first_module>();
        ASSERT_NE(first_module.logger(), nullptr);
        std::filesystem::path first_module_log_file = app.log_dir() / "first_module.log";
        ASSERT_TRUE(std::filesystem::exists(first_module_log_file));
    }
    {
        ut::application app;
        ASSERT_TRUE(std::filesystem::exists(app.log_dir()));
        ASSERT_NE(app.logger(), nullptr);
        std::filesystem::path app_log_file = app.log_dir() / "application.log";
        ASSERT_TRUE(std::filesystem::exists(app_log_file));
        ut::first_module& first_module = app.create_module<ut::first_module>();
        ASSERT_NE(first_module.logger(), nullptr);
        std::filesystem::path first_module_log_file = app.log_dir() / "first_module.log";
        ASSERT_TRUE(std::filesystem::exists(first_module_log_file));
    }
}

TEST(spdlogging_tests, test_logs)
{
    ut::application app(core::program_args(pargc, pargv));
    ASSERT_TRUE(std::filesystem::exists(app.log_dir()));
    ASSERT_NE(app.logger(), nullptr);
    std::filesystem::path app_log_file = app.log_dir() / "program_name.v2.log";
    ASSERT_TRUE(std::filesystem::exists(app_log_file));

    app.create_main_module<ut::times_up_module>();
    ut::first_module& first_module = app.create_module<ut::first_module>();
    first_module.set_frequency(2);
    ut::second_module& second_module = app.create_module<ut::second_module>();
    second_module.set_frequency(3);
    ASSERT_NE(first_module.logger(), nullptr);
    ASSERT_NE(second_module.logger(), nullptr);
    std::filesystem::path first_module_log_file = app.log_dir() / "first_module.log";
    std::filesystem::path second_module_log_file = app.log_dir() / "second_module.log";
    ASSERT_TRUE(std::filesystem::exists(first_module_log_file));
    ASSERT_TRUE(std::filesystem::exists(second_module_log_file));
    ASSERT_EQ(std::filesystem::file_size(app_log_file), 0);
    ASSERT_EQ(std::filesystem::file_size(first_module_log_file), 0);
    ASSERT_EQ(std::filesystem::file_size(second_module_log_file), 0);

    app.init();
    std::ignore = app.run();
    app.logger()->flush();
    first_module.logger()->flush();
    second_module.logger()->flush();
    ASSERT_TRUE(std::filesystem::exists(app_log_file));
    ASSERT_TRUE(std::filesystem::exists(first_module_log_file));
    ASSERT_TRUE(std::filesystem::exists(second_module_log_file));
    {
        std::ifstream stream(app_log_file);
    }
    {
        std::ifstream stream(first_module_log_file);
    }
    {
        std::ifstream stream(second_module_log_file);
    }
    ASSERT_GT(std::filesystem::file_size(app_log_file), 0);
    ASSERT_GT(std::filesystem::file_size(first_module_log_file), 0);
    ASSERT_GT(std::filesystem::file_size(second_module_log_file), 0);
}

int main(int argc, char** argv)
{
    std::filesystem::create_directories(program_dir);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
