#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <appt/application/multi_task_application.hpp>
#include <appt/application/program_args.hpp>
#include <appt/application/module/loop_module.hpp>
#include <appt/util/logger_helper.hpp>
#include <appt/application/decorator/logging.hpp>
#include <appt/application/module/decorator/logging.hpp>

namespace example
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
        SPDLOG_LOGGER_TRACE(app().logger(), "Test");
        SPDLOG_LOGGER_DEBUG(app().logger(), "Test");
        SPDLOG_LOGGER_INFO(app().logger(), "Test");
        SPDLOG_LOGGER_WARN(app().logger(), "Test");
        SPDLOG_LOGGER_ERROR(app().logger(), "Test");

        SPDLOG_LOGGER_INFO(app().logger(), "start!");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        app().stop_side_modules();
        SPDLOG_LOGGER_CRITICAL(app().logger(), "end!");
    }
};

class first_module : public appt::mdec::logging<appt::module_logger, appt::loop_module<first_module, application>>
{
private:
    using base_ = appt::mdec::logging<appt::module_logger, appt::loop_module<first_module, application>>;

public:
    first_module() : base_("first_module") {}
    virtual ~first_module() override = default;

    void run_loop(appt::seconds)
    {
        SPDLOG_LOGGER_INFO(logger(), "Yo!");
    }

    virtual void finish() override
    {
        SPDLOG_LOGGER_CRITICAL(logger(), "finish");
    }
};

class second_module : public appt::mdec::logging<appt::module_logger, appt::loop_module<second_module, application>>
{
private:
    using base_ = appt::mdec::logging<appt::module_logger, appt::loop_module<second_module, application>>;

public:
    second_module() : base_("second_module") {}
    virtual ~second_module() override = default;

    virtual void init() override
    {
        logger()->set_level(spdlog::level::trace);
    }

    void run_loop(appt::seconds)
    {
        SPDLOG_LOGGER_INFO(logger(), "start!");
        coucou<int>();
        coucou<float>();
        SPDLOG_LOGGER_ERROR(logger(), "end!");
    }

    template <class Type>
    void coucou()
    {
        SPDLOG_LOGGER_DEBUG(logger(), "coucou");
    }

    virtual void finish() override
    {
        SPDLOG_LOGGER_CRITICAL(logger(), "finish");
    }
};

}

int main(int argc, char** argv)
{
    example::application app(argc, argv);
    app.create_main_module<example::times_up_module>();
    app.create_module<example::first_module>().set_frequency(2);
    app.create_module<example::second_module>().set_frequency(3);
    app.init();
    app.run();
    SPDLOG_LOGGER_INFO(app.logger(), "EXIT SUCCESS");

    return EXIT_SUCCESS;
}
