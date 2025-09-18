#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <arba/appt/application/standard/application.hpp>
#include <arba/appt/application/decorator/spdlogging/spdlogging.hpp>
#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/application/decorator/multi_user/multi_user.hpp>
#include <arba/appt/application/module/decorator/spdlogging/spdlogging.hpp>
#include <arba/appt/application/module/decorator/loop.hpp>
#include <arba/appt/application/module/decorator/multi_user/multi_user.hpp>
#include <arba/appt/application/module/standard/module.hpp>
#include <arba/appt/util/spdlogging/logging_macro.hpp>

#include <random>

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

class application_base
{
private:
    application_base() = delete;
    using logging_application_ = appt::adec::spdlogging<appt::application<>>;
    using multi_user_application_ = appt::adec::multi_user<user, logging_application_>;
    using multi_task_application_ = appt::adec::multi_task<multi_user_application_>;

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
        logger()->set_level(spdlog::level::trace);
        ARBA_APPT_SPDLOGGER_TRACE(logger());
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

namespace priv
{
using module_ = appt::module<application>;
using logging_module_ = appt::mdec::spdlogging<module_>;
using multi_user_logging_module_ = appt::mdec::multi_user<user, appt::user_sptr_name_hash<user>, logging_module_>;
} // namespace priv

template <class module_type>
using loop_multi_user_logging_module = appt::mdec::loop<priv::multi_user_logging_module_, module_type>;
template <class module_type>
using loop_logging_module = appt::mdec::loop<priv::logging_module_, module_type>;

class consumer_module : public loop_multi_user_logging_module<consumer_module>,
                        public evnt::event_listener<number_event>
{
private:
    using base_ = loop_multi_user_logging_module<consumer_module>;

public:
    consumer_module(application_type& app) : base_(app, "consumer_module") {}
    virtual ~consumer_module() override = default;

    virtual void init() override
    {
        this->base_::init();
        logger()->set_level(spdlog::level::trace);
        event_manager().connect<number_event>(*this);
        users().reserve(4);
    }

    void run_loop(appt::dt::seconds)
    {
        ARBA_APPT_SPDLOGGER_TRACE(logger());
        event_manager().emit(event_box());

        for (const auto& user_sptr : users())
            SPDLOG_LOGGER_INFO(logger(), "'{}'", user_sptr->name());
        if (users().size() >= 4)
            stop();
    }

    virtual void finish() override
    {
        ARBA_APPT_SPDLOGGER_TRACE(logger());
        app().stop_side_modules();
    }

    void receive(number_event& event)
    {
        ARBA_APPT_SPDLOGGER_TRACE(logger());

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

class generator_module : public loop_logging_module<generator_module>
{
private:
    using base_ = loop_logging_module<generator_module>;

public:
    generator_module(application_type& app) : base_(app, "generator_module"), int_generator_(std::random_device{}()) {}
    virtual ~generator_module() override = default;

    virtual void init() override
    {
        this->base_::init();
        logger()->set_level(spdlog::level::trace);
    }

    void run_loop(appt::dt::seconds)
    {
        ARBA_APPT_SPDLOGGER_TRACE(logger());

        number_event event{ die100() };
        app().event_manager().emit(event);
    }

    virtual void finish() override { ARBA_APPT_SPDLOGGER_TRACE(logger()); }

private:
    unsigned die100()
    {
        static std::uniform_int_distribution<> die(1, 100);
        return die(int_generator_);
    }

private:
    std::mt19937_64 int_generator_;
};

} // namespace example

int main(int argc, char** argv)
{
    example::application app(core::program_args(argc, argv));
    app.create_main_module<example::consumer_module>().set_frequency(3);
    app.create_module<example::generator_module>().set_frequency(2);
    app.init();
    int res = app.run();

    SPDLOG_LOGGER_INFO(app.logger(), "EXIT");
    return res;
}
