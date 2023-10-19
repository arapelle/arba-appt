#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <arba/appt/application/application.hpp>
#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/application/decorator/logging.hpp>
#include <arba/appt/application/module/module.hpp>
#include <arba/appt/application/module/decorator/loop.hpp>
#include <arba/appt/application/module/decorator/logging.hpp>
#include <arba/appt/util/format_time_point.hpp>
#include <gtest/gtest.h>
#include <cstdlib>
#include <fstream>

using namespace std::string_literals;

const std::filesystem::path program_dir = std::filesystem::temp_directory_path() / "root/dir";
std::array s_args = { (program_dir / "program_name.v2.run").generic_string(), "6"s, "-c"s, "Debug"s };
std::array cs_args = { s_args[0].data(), s_args[1].data(), s_args[2].data(), s_args[3].data() };
int argc = cs_args.size();
char** argv = cs_args.data();

namespace ut
{
class application_base
{
private:
    application_base() = delete;
    using logging_application_ = appt::adec::logging<appt::application_logger, appt::application<>>;
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

    void init()
    {
        base_::init();
        logger()->set_level(spdlog::level::trace);
    }
};

class times_up_module : public appt::module<application>
{
public:
    virtual ~times_up_module() override = default;

    virtual void run() override
    {
        SPDLOG_LOGGER_TRACE(app().logger(), "");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        app().stop_side_modules();
    }
};

class first_module : public appt::mdec::loop<appt::mdec::logging<appt::module_logger, appt::module<application>>, first_module>
{
private:
    using base_ = appt::mdec::loop<appt::mdec::logging<appt::module_logger, appt::module<application>>, first_module>;

public:
    first_module() : base_("first_module") {}
    virtual ~first_module() override = default;

    void run_loop(appt::seconds)
    {
        SPDLOG_LOGGER_INFO(logger(), "");
    }

    virtual void finish() override
    {
        SPDLOG_LOGGER_INFO(logger(), "");
    }
};

class second_module : public appt::mdec::loop<appt::mdec::logging<appt::module_logger, appt::module<application>>, second_module>
{
private:
    using base_ = appt::mdec::loop<appt::mdec::logging<appt::module_logger, appt::module<application>>, second_module>;

public:
    second_module() : base_("second_module") {}
    virtual ~second_module() override = default;

    virtual void init() override
    {
        logger()->set_level(spdlog::level::debug);
    }

    void run_loop(appt::seconds)
    {
        SPDLOG_LOGGER_DEBUG(logger(), "");
    }

    virtual void finish() override
    {
        SPDLOG_LOGGER_DEBUG(logger(), "");
    }
};

}

TEST(logging_tests, test_logs__no_args)
{
    {
        ut::application app;
        ASSERT_TRUE(std::filesystem::exists(app.log_dir()));
        ASSERT_NE(app.logger(), nullptr);
        ut::first_module& first_module = app.create_module<ut::first_module>();
        ASSERT_NE(first_module.logger(), nullptr);
        std::filesystem::path first_module_log_file = app.log_dir()/"first_module.log";
        ASSERT_TRUE(std::filesystem::exists(first_module_log_file));
    }
    {
        ut::application app;
        ASSERT_TRUE(std::filesystem::exists(app.log_dir()));
        ASSERT_NE(app.logger(), nullptr);
        ut::first_module& first_module = app.create_module<ut::first_module>();
        ASSERT_NE(first_module.logger(), nullptr);
        std::filesystem::path first_module_log_file = app.log_dir()/"first_module.log";
        ASSERT_TRUE(std::filesystem::exists(first_module_log_file));
    }
}

TEST(logging_tests, test_logs)
{
    ut::application app(appt::program_args(argc, argv));
    app.set_log_dir(std::filesystem::path("./logs/") / appt::format_filename_Ymd_HMS());
    ASSERT_TRUE(std::filesystem::exists(app.log_dir()));
    ASSERT_NE(app.logger(), nullptr);
    std::filesystem::path times_up_log_file = app.log_dir()/"program_name.v2.log";
    ASSERT_TRUE(std::filesystem::exists(times_up_log_file));

    app.create_main_module<ut::times_up_module>();
    ut::first_module& first_module = app.create_module<ut::first_module>();
    first_module.set_frequency(2);
    ut::second_module& second_module = app.create_module<ut::second_module>();
    second_module.set_frequency(3);
    ASSERT_NE(first_module.logger(), nullptr);
    ASSERT_NE(second_module.logger(), nullptr);
    std::filesystem::path first_module_log_file = app.log_dir()/"first_module.log";
    std::filesystem::path second_module_log_file = app.log_dir()/"second_module.log";
    ASSERT_TRUE(std::filesystem::exists(first_module_log_file));
    ASSERT_TRUE(std::filesystem::exists(second_module_log_file));
    ASSERT_EQ(std::filesystem::file_size(times_up_log_file), 0);
    ASSERT_EQ(std::filesystem::file_size(first_module_log_file), 0);
    ASSERT_EQ(std::filesystem::file_size(second_module_log_file), 0);

    app.init();
    std::ignore = app.run();
    app.logger()->flush();
    first_module.logger()->flush();
    second_module.logger()->flush();
    ASSERT_TRUE(std::filesystem::exists(times_up_log_file));
    ASSERT_TRUE(std::filesystem::exists(first_module_log_file));
    ASSERT_TRUE(std::filesystem::exists(second_module_log_file));
    { std::ifstream stream(times_up_log_file); }
    { std::ifstream stream(first_module_log_file); }
    { std::ifstream stream(second_module_log_file); }
    ASSERT_GT(std::filesystem::file_size(times_up_log_file), 0);
    ASSERT_GT(std::filesystem::file_size(first_module_log_file), 0);
    ASSERT_GT(std::filesystem::file_size(second_module_log_file), 0);
}

int main(int argc, char** argv)
{
    std::filesystem::create_directories(program_dir);
    ::testing::InitGoogleTest(&argc, argv);
    auto res = RUN_ALL_TESTS();
    std::filesystem::remove_all(program_dir);
    return res;
}
