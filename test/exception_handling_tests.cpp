#include <arba/appt/application/application.hpp>
#include <arba/appt/application/decorator/logging.hpp>
#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/application/module/module.hpp>
#include <arba/appt/application/module/decorator/logging.hpp>
#include <arba/appt/application/module/decorator/loop.hpp>
#include <gtest/gtest.h>

using logging_application = appt::adec::logging<appt::application_logger, appt::application<>>;
using multi_task_application = appt::adec::multi_task<appt::application<>>;
using multi_task_logging_application = appt::adec::multi_task<logging_application>;

template <class ApplicationBaseType>
class ut_application : public ApplicationBaseType::template rebind_t<ut_application<ApplicationBaseType>>
{
    using base_ = ApplicationBaseType::template rebind_t<ut_application>;

public:
    using base_::base_;
};

template <class ModuleBaseType>
class ut_counting_module : public ModuleBaseType::template rebind_t<ut_counting_module<ModuleBaseType>>
{
private:
    using base_ = ModuleBaseType::template rebind_t<ut_counting_module>;

public:
    ut_counting_module() : base_("ut_counting_module") {}
    virtual ~ut_counting_module() override = default;

    virtual void init() override
    {
        ++init_count;
    }

    void run_loop(appt::seconds /*delta_time*/)
    {
        ++run_count;
        if (run_count >= 10)
            this->stop();
    }

    virtual void finish() override
    {
        ++finish_count;
    }

    uint16_t init_count = 0;
    uint16_t run_count = 0;
    uint16_t finish_count = 0;
};

template <class ModuleBaseType>
class ut_failing_module : public ModuleBaseType::template rebind_t<ut_failing_module<ModuleBaseType>>
{
private:
    using base_ = ModuleBaseType::template rebind_t<ut_failing_module>;

public:
    ut_failing_module() : base_("ut_failing_module") {}
    virtual ~ut_failing_module() override = default;

    virtual void init() override
    {
    }

    void run_loop(appt::seconds /*delta_time*/)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        throw std::runtime_error("FAIL");
    }

    virtual void finish() override
    {
    }
};

std::string file_content(const std::filesystem::path& fpath)
{
    std::string contents;
    std::ifstream stream(fpath);
    stream.seekg(0, std::ios::end);
    contents.reserve(stream.tellg());
    stream.seekg(0, std::ios::beg);
    contents.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    return contents;
}

TEST(exception_handling_tests, test_main_module_fail__logging)
{
    using app_type = ut_application<multi_task_logging_application>;
    using mod_type = appt::mdec::loop<appt::module<app_type>>;

    app_type app;
    auto& main_module = app.create_main_module<ut_failing_module<mod_type>>();
    auto& loop_module = app.create_module<ut_counting_module<mod_type>>();
    loop_module.set_frequency(6);
    app.init();
    ASSERT_EQ(app.run(), appt::execution_failure);

    app.logger()->flush();

    std::filesystem::path log_fpath = app.log_dir() / (app.logger()->name() + ".log");
    ASSERT_TRUE(std::filesystem::exists(log_fpath));
    std::string log_file_content = file_content(log_fpath);
    ASSERT_NE(log_file_content.find("[critical]"), std::string::npos);
    ASSERT_NE(log_file_content.find("FAIL"), std::string::npos);
}

TEST(exception_handling_tests, test_main_module_fail__printing)
{
    using app_type = ut_application<multi_task_application>;
    using mod_type = appt::mdec::loop<appt::module<app_type>>;

    app_type app;
    auto& main_module = app.create_main_module<ut_failing_module<mod_type>>();
    auto& loop_module = app.create_module<ut_counting_module<mod_type>>();
    loop_module.set_frequency(6);
    app.init();
    ASSERT_EQ(app.run(), appt::execution_failure);
}

int main(int argc, char** argv)
{
    const std::filesystem::path program_dir = std::filesystem::temp_directory_path() / "application";
    core::sbrm program_dir_remover = core::make_sb_all_files_remover(program_dir);
    std::filesystem::create_directories(program_dir);

    ::testing::InitGoogleTest(&argc, argv);
    int exit_value = RUN_ALL_TESTS();
    if (exit_value != 0)
        program_dir_remover.disable();

    return exit_value;
}
