#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <arba/appt/application/basic_application.hpp>
#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/application/module/basic_module.hpp>
#include <arba/appt/application/module/decorator/loop.hpp>
#include <arba/appt/spdlogging/application/decorator/spdlogging.hpp>
#include <arba/appt/spdlogging/application/module/decorator/spdlogging.hpp>
#include <arba/appt/spdlogging/util/logging_macro.hpp>

#include <random>

namespace example
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
using module_ = appt::basic_module<application>;
using logging_module_ = appt::mdec::spdlogging<module_>;
} // namespace priv

template <class module_type>
using loop_multi_user_logging_module = appt::mdec::loop<priv::logging_module_, module_type>;
template <class module_type>
using loop_logging_module = appt::mdec::loop<priv::logging_module_, module_type>;

class consumer_module : public loop_multi_user_logging_module<consumer_module>
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
        users_.reserve(4);
    }

    void run_loop(appt::dt::seconds)
    {
        ARBA_APPT_SPDLOGGER_TRACE(logger());

        std::lock_guard lock(mutex_);
        for (const auto& user_name : users_)
            SPDLOG_LOGGER_INFO(logger(), "'{}'", user_name);
        if (users_.size() >= 4)
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

        std::lock_guard lock(mutex_);
        if (users_.size() < 4)
        {
            SPDLOG_LOGGER_INFO(logger(), "received {}", event.number);
            std::string name;
            {
                std::ostringstream stream;
                stream << "User#" << event.number;
                name = stream.str();
            }
            if (std::find(users_.cbegin(), users_.cend(), name) == users_.cend())
                users_.emplace_back(std::move(name));
        }
    }

private:
    std::vector<std::string> users_;
    std::mutex mutex_;
};

class generator_module : public loop_logging_module<generator_module>
{
private:
    using base_ = loop_logging_module<generator_module>;

public:
    generator_module(application_type& app) : base_(app, "generator_module"), int_generator_(std::random_device{}()) {}
    virtual ~generator_module() override = default;

    void set_consumer_module(consumer_module& module)
    {
        assert(!is_running());
        consumer_module_ = &module;
    }

    virtual void init() override
    {
        this->base_::init();
        logger()->set_level(spdlog::level::trace);
        assert(consumer_module_);
    }

    void run_loop(appt::dt::seconds)
    {
        ARBA_APPT_SPDLOGGER_TRACE(logger());

        number_event event{ die100() };
        consumer_module_->receive(event);
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
    consumer_module* consumer_module_ = nullptr;
};

} // namespace example

int main(int argc, char** argv)
{
    example::application app(core::program_args(argc, argv));
    auto& cons_module = app.create_main_module<example::consumer_module>();
    cons_module.set_frequency(3);
    auto& gen_module = app.create_module<example::generator_module>();
    gen_module.set_frequency(2);
    gen_module.set_consumer_module(cons_module);
    app.init();
    int res = app.run();

    SPDLOG_LOGGER_INFO(app.logger(), "EXIT");
    return res;
}
