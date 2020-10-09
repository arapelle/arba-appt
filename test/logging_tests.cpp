#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <appt/application/multi_task_application.hpp>
#include <appt/application/module/loop_module.hpp>
#include <appt/application/decorator/logging.hpp>
#include <appt/application/module/decorator/logging.hpp>
#include <gtest/gtest.h>
#include <cstdlib>

using namespace std::string_literals;

std::array s_args = { "/root/dir/program_name.v2.run"s, "6"s, "-c"s, "Debug"s };
std::array cs_args = { s_args[0].data(), s_args[1].data(), s_args[2].data(), s_args[3].data() };
int argc = cs_args.size();
char** argv = cs_args.data();

namespace ut
{
class application : public appt::adec::logging<appt::application_logger, appt::multi_task_application<application>>
{
private:
    using base_ = appt::adec::logging<appt::application_logger, appt::multi_task_application<application>>;

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

class first_module : public appt::mdec::logging<appt::module_logger, appt::loop_module<application, first_module>>
{
private:
    using base_ = appt::mdec::logging<appt::module_logger, appt::loop_module<application, first_module>>;

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

class second_module : public appt::mdec::logging<appt::module_logger, appt::loop_module<application, second_module>>
{
private:
    using base_ = appt::mdec::logging<appt::module_logger, appt::loop_module<application, second_module>>;

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

TEST(logging_tests, test_logs)
{
    ut::application app(argc, argv);
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
    app.run();
    app.logger()->flush();
    first_module.logger()->flush();
    second_module.logger()->flush();
    ASSERT_GT(std::filesystem::file_size(times_up_log_file), 0);
    ASSERT_GT(std::filesystem::file_size(first_module_log_file), 0);
    ASSERT_GT(std::filesystem::file_size(second_module_log_file), 0);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
