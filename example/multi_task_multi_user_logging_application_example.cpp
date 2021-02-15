#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <appt/application/application.hpp>
#include <appt/application/decorator/logging.hpp>
#include <appt/application/decorator/multi_task.hpp>
#include <appt/application/decorator/multi_user.hpp>
#include <appt/application/module/module.hpp>
#include <appt/application/module/decorator/logging.hpp>
#include <appt/application/module/decorator/multi_user.hpp>
#include <appt/application/module/decorator/loop.hpp>
#include <appt/util/logging_macro.hpp>
#include <random>
#include <iostream>

namespace example
{
class user : public appt::user
{
public:
    virtual ~user() = default;

    user(const std::string& name = "") : name_(name) {}
    const std::string& name() const { return name_; }

public:
    std::string name_;
};

using logging_application = appt::adec::logging<appt::application_logger, appt::application>;
using multi_user_application = appt::adec::multi_user<user, logging_application>;

class application : public appt::adec::multi_task<multi_user_application, application>
{
private:
    using base_ = appt::adec::multi_task<multi_user_application, application>;

public:
    using base_::base_;

    void init()
    {
        logger()->set_level(spdlog::level::trace);
        ARBA_APPT_LOGGER_TRACE(logger());
        SPDLOG_LOGGER_DEBUG(logger(), "Test");
        SPDLOG_LOGGER_INFO(logger(), "Test");
        SPDLOG_LOGGER_WARN(logger(), "Test");
        SPDLOG_LOGGER_ERROR(logger(), "Test");
        base_::init();
    }
};

struct number_event
{
    unsigned number;
};

using logging_module = appt::mdec::logging<appt::module_logger, appt::module<application>>;
using multi_user_module = appt::mdec::multi_user<user, appt::user_sptr_name_hash<user>, logging_module>;
template <class module_type>
using loop_multi_user_module = appt::mdec::loop<multi_user_module, module_type>;

class consumer_module : public loop_multi_user_module<consumer_module>,
                        public evnt::event_listener<number_event>
{
private:
    using base_ = loop_multi_user_module<consumer_module>;

public:
    consumer_module() : base_("consumer_module") {}
    virtual ~consumer_module() override = default;

    virtual void init() override
    {
        logger()->set_level(spdlog::level::trace);
        event_manager().connect<number_event>(*this);
        users().reserve(4);
    }

    void run_loop(appt::seconds)
    {
        ARBA_APPT_LOGGER_TRACE(logger());
        event_manager().emit(event_box());

        for (const auto& user_sptr : users())
            SPDLOG_LOGGER_INFO(logger(), "'{}'", user_sptr->name());
        if (users().size() >= 4)
            stop();
    }

    virtual void finish() override
    {
        ARBA_APPT_LOGGER_TRACE(logger());
    }

    void receive(number_event& event)
    {
        ARBA_APPT_LOGGER_TRACE(logger());

        if (users().size() < 4)
        {
            SPDLOG_LOGGER_INFO(logger(), "received {}", event.number);
            std::string name;
            {
                std::ostringstream stream;
                stream << "User#" << event.number;
                name = stream.str();
            }
            if (users().find_user(name) == users().end())
                users().create_user(name);
        }
    }
};

class generator_module : public loop_multi_user_module<generator_module>
{
private:
    using base_ = loop_multi_user_module<generator_module>;

public:
    generator_module() : base_("generator_module"), int_generator_(std::random_device{}()) {}
    virtual ~generator_module() override = default;

    virtual void init() override
    {
        logger()->set_level(spdlog::level::trace);
    }

    void run_loop(appt::seconds)
    {
        ARBA_APPT_LOGGER_TRACE(logger());

        number_event event{ die100() };
        app().event_manager().emit(event);
    }

    virtual void finish() override
    {
        ARBA_APPT_LOGGER_TRACE(logger());
    }

private:
    unsigned die100()
    {
        static std::uniform_int_distribution<> die(1, 100);
        return die(int_generator_);
    }

private:
    std::mt19937_64 int_generator_;
};

}

int main(int argc, char** argv)
{
    example::application app(argc, argv);
    app.create_main_module<example::consumer_module>().set_frequency(3);
    app.create_module<example::generator_module>().set_frequency(2);
    app.init();
    app.run();

    SPDLOG_LOGGER_INFO(app.logger(), "EXIT SUCCESS");
    return EXIT_SUCCESS;
}