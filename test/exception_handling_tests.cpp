#include "modules/bad_crtp_module.hpp"
#include "util/stream_capture.hpp"
#include "util/text_file_content.hpp"
#include <arba/appt/application/application.hpp>
#include <arba/appt/application/decorator/logging.hpp>
#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/application/module/decorator/logging.hpp>
#include <arba/appt/application/module/decorator/loop.hpp>
#include <arba/appt/application/module/module.hpp>

#include <gtest/gtest.h>

using logging_application = appt::adec::logging<appt::application<>>;
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
    using typename base_::application_type;

    ut_counting_module(application_type& app) : base_(app, "ut_counting_module") {}
    virtual ~ut_counting_module() override = default;

    virtual void init() override
    {
        this->base_::init();
        ++init_count;
    }

    void run_loop(appt::dt::seconds /*delta_time*/)
    {
        ++run_count;
        if (run_count >= 30)
            std::abort();
    }

    virtual void finish() override { ++finish_count; }

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
    using typename base_::application_type;

    ut_failing_module(application_type& app) : base_(app, "ut_failing_module") {}
    virtual ~ut_failing_module() override = default;

    virtual void init() override
    {
        if (!base_init_not_called)
            this->base_::init();
        if (init_fails)
            throw std::runtime_error("INIT_FAIL");
    }

    void run_loop(appt::dt::seconds)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (run_fails)
            throw std::runtime_error("RUN_FAIL");
    }

    bool base_init_not_called = false;
    bool init_fails = false;
    bool run_fails = false;
};

// Main module tests

TEST(exception_handling_tests, test_main_module_init_fails__logging)
{
    using app_type = ut_application<multi_task_logging_application>;
    using mod_type = appt::mdec::loop<appt::module<app_type>>;

    app_type app;
    auto& main_module = app.create_main_module<ut_failing_module<mod_type>>();
    main_module.init_fails = true;
    auto& loop_module = app.create_module<ut_counting_module<mod_type>>();
    loop_module.set_frequency(20);
    ASSERT_EQ(app.init(), appt::execution_failure);
    ASSERT_EQ(app.run(), appt::execution_failure);

    app.logger()->flush();
    app.logger()->flush_on(spdlog::level::critical);

    std::filesystem::path log_fpath = app.log_path();
    ASSERT_TRUE(std::filesystem::exists(log_fpath));
    std::string log_file_contents = text_file_content(log_fpath);
    ASSERT_NE(log_file_contents.find("[critical]"), std::string::npos);
    ASSERT_NE(log_file_contents.find("INIT_FAIL"), std::string::npos);
}

TEST(exception_handling_tests, test_main_module_init_fails__printing)
{
    using app_type = ut_application<multi_task_application>;
    using mod_type = appt::mdec::loop<appt::module<app_type>>;

    stream_capture cerr_capture(std::cerr);

    app_type app;
    auto& main_module = app.create_main_module<ut_failing_module<mod_type>>();
    main_module.init_fails = true;
    auto& loop_module = app.create_module<ut_counting_module<mod_type>>();
    loop_module.set_frequency(20);
    ASSERT_EQ(app.init(), appt::execution_failure);
    ASSERT_EQ(app.run(), appt::execution_failure);

    std::string log_stream_contents = cerr_capture.str();
    ASSERT_NE(log_stream_contents.find("[critical]"), std::string::npos);
    ASSERT_NE(log_stream_contents.find("INIT_FAIL"), std::string::npos);
}

TEST(exception_handling_tests, test_main_module_run_fails__logging)
{
    using app_type = ut_application<multi_task_logging_application>;
    using mod_type = appt::mdec::loop<appt::module<app_type>>;

    app_type app;
    auto& main_module = app.create_main_module<ut_failing_module<mod_type>>();
    main_module.run_fails = true;
    auto& loop_module = app.create_module<ut_counting_module<mod_type>>();
    loop_module.set_frequency(20);
    ASSERT_EQ(app.init(), appt::execution_success);
    ASSERT_EQ(app.run(), appt::execution_failure);

    app.logger()->flush();
    app.logger()->flush_on(spdlog::level::critical);

    std::filesystem::path log_fpath = app.log_path();
    ASSERT_TRUE(std::filesystem::exists(log_fpath));
    std::string log_file_contents = text_file_content(log_fpath);
    ASSERT_NE(log_file_contents.find("[critical]"), std::string::npos);
    ASSERT_NE(log_file_contents.find("RUN_FAIL"), std::string::npos);
}

TEST(exception_handling_tests, test_main_module_run_fails__printing)
{
    using app_type = ut_application<multi_task_application>;
    using mod_type = appt::mdec::loop<appt::module<app_type>>;

    stream_capture cerr_capture(std::cerr);

    app_type app;
    auto& main_module = app.create_main_module<ut_failing_module<mod_type>>();
    main_module.run_fails = true;
    auto& loop_module = app.create_module<ut_counting_module<mod_type>>();
    loop_module.set_frequency(20);
    ASSERT_EQ(app.init(), appt::execution_success);
    ASSERT_EQ(app.run(), appt::execution_failure);

    std::string log_stream_contents = cerr_capture.str();
    ASSERT_NE(log_stream_contents.find("[critical]"), std::string::npos);
    ASSERT_NE(log_stream_contents.find("RUN_FAIL"), std::string::npos);
}

TEST(exception_handling_tests, test_main_module_run_fails__init_not_called_logging)
{
    using app_type = ut_application<multi_task_logging_application>;

    app_type app;
    ASSERT_EQ(app.run(), appt::execution_failure);

    app.logger()->flush();

    std::filesystem::path log_fpath = app.log_path();
    ASSERT_TRUE(std::filesystem::exists(log_fpath));
    std::string log_file_contents = text_file_content(log_fpath);
    ASSERT_NE(log_file_contents.find("[critical]"), std::string::npos);
    ASSERT_NE(log_file_contents.find("Did you forget to call init()?"), std::string::npos);
}

TEST(exception_handling_tests, test_main_module_init_fails__base_init_not_called_logging)
{
    using app_type = ut_application<multi_task_logging_application>;
    using mod_type = appt::mdec::loop<appt::module<app_type>>;

    app_type app;
    auto& main_module = app.create_main_module<ut_failing_module<mod_type>>();
    main_module.base_init_not_called = true;
    auto& loop_module = app.create_module<ut_counting_module<mod_type>>();
    loop_module.set_frequency(20);
    ASSERT_EQ(app.init(), appt::execution_failure);
    ASSERT_EQ(app.run(), appt::execution_failure);

    app.logger()->flush();

    std::filesystem::path log_fpath = app.log_path();
    ASSERT_TRUE(std::filesystem::exists(log_fpath));
    std::string log_file_contents = text_file_content(log_fpath);
    ASSERT_NE(log_file_contents.find("[critical]"), std::string::npos);
    ASSERT_NE(log_file_contents.find("The init status is 'ready' (should be at least 'executing'). "
                                     "Did you forget to call parent init()?"),
              std::string::npos);
}

TEST(exception_handling_tests, test_main_module_init_fails__bad_derived_module)
{
    using app_type = ut_application<multi_task_logging_application>;

    app_type app;
    app_type::module_base_uptr mod_uptr = std::make_unique<ut::bad_crtp_module<app_type>>(std::ref(app));
    app.set_main_module(std::move(mod_uptr));
    ASSERT_EQ(app.init(), appt::execution_failure);
    ASSERT_EQ(app.run(), appt::execution_failure);

    app.logger()->flush();

    std::filesystem::path log_fpath = app.log_path();
    ASSERT_TRUE(std::filesystem::exists(log_fpath));
    std::string log_file_contents = text_file_content(log_fpath);
    ASSERT_NE(log_file_contents.find("[critical]"), std::string::npos);
    ASSERT_NE(log_file_contents.find("CRTP class is not used correctly."), std::string::npos);
}

// Side module tests

TEST(exception_handling_tests, test_side_module_run_fails__log_to_module_logger)
{
    using app_type = ut_application<multi_task_logging_application>;
    using mod_type = appt::mdec::loop<appt::mdec::logging<appt::module<app_type>>>;

    app_type app;
    auto& main_module = app.create_main_module<ut_counting_module<mod_type>>();
    main_module.set_frequency(20);
    auto& side_module = app.create_module<ut_failing_module<mod_type>>();
    side_module.run_fails = true;
    ASSERT_EQ(app.init(), appt::execution_success);
    ASSERT_EQ(app.run(), appt::execution_failure);

    side_module.logger()->flush();

    std::filesystem::path log_fpath = side_module.log_path();
    ASSERT_TRUE(std::filesystem::exists(log_fpath));
    std::string log_file_contents = text_file_content(log_fpath);
    ASSERT_NE(log_file_contents.find("[critical]"), std::string::npos);
    ASSERT_NE(log_file_contents.find("RUN_FAIL"), std::string::npos);
}

TEST(exception_handling_tests, test_side_module_run_fails__log_to_app_logger)
{
    using app_type = ut_application<multi_task_logging_application>;
    using mod_type = appt::mdec::loop<appt::module<app_type>>;

    app_type app;
    auto& main_module = app.create_main_module<ut_counting_module<mod_type>>();
    main_module.set_frequency(20);
    auto& side_module = app.create_module<ut_failing_module<mod_type>>();
    side_module.run_fails = true;
    ASSERT_EQ(app.init(), appt::execution_success);
    ASSERT_EQ(app.run(), appt::execution_failure);

    app.logger()->flush();
    app.logger()->flush_on(spdlog::level::critical);

    std::filesystem::path log_fpath = app.log_path();
    ASSERT_TRUE(std::filesystem::exists(log_fpath));
    std::string log_file_contents = text_file_content(log_fpath);
    ASSERT_NE(log_file_contents.find("[critical]"), std::string::npos);
    ASSERT_NE(log_file_contents.find("RUN_FAIL"), std::string::npos);
}

TEST(exception_handling_tests, test_side_module_run_fails__log_to_cerr)
{
    using app_type = ut_application<multi_task_application>;
    using mod_type = appt::mdec::loop<appt::module<app_type>>;

    stream_capture cerr_capture(std::cerr);

    app_type app;
    auto& main_module = app.create_main_module<ut_counting_module<mod_type>>();
    main_module.set_frequency(20);
    auto& side_module = app.create_module<ut_failing_module<mod_type>>();
    side_module.run_fails = true;
    ASSERT_EQ(app.init(), appt::execution_success);
    ASSERT_EQ(app.run(), appt::execution_failure);

    std::string log_stream_contents = cerr_capture.str();
    ASSERT_NE(log_stream_contents.find("[critical]"), std::string::npos);
    ASSERT_NE(log_stream_contents.find("RUN_FAIL"), std::string::npos);
}

int main(int argc, char** argv)
{
    const std::filesystem::path program_dir = std::filesystem::temp_directory_path() / "application";
    std::filesystem::create_directories(program_dir);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
