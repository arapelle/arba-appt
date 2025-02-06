#include <arba/appt/application/application.hpp>
#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/application/decorator/multi_user.hpp>
#include <arba/appt/application/module/decorator/loop.hpp>
#include <arba/appt/application/module/decorator/multi_user.hpp>
#include <arba/appt/application/module/module.hpp>
#include <arba/core/program_args.hpp>

#include <iostream>
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

using multi_user_application = appt::adec::multi_user<user, appt::application<>>;

class application : public appt::adec::multi_task<multi_user_application, application>
{
private:
    using base_ = appt::adec::multi_task<multi_user_application, application>;

public:
    using base_::base_;

    void init() { base_::init(); }
};

struct number_event
{
    unsigned number;
};

using module = appt::module<application>;
using multi_user_module = appt::mdec::multi_user<user, appt::user_sptr_name_hash<user>, module>;
template <class module_type>
using loop_multi_user_module = appt::mdec::loop<multi_user_module, module_type>;

class consumer_module : public loop_multi_user_module<consumer_module>, public evnt::event_listener<number_event>
{
private:
    using base_ = loop_multi_user_module<consumer_module>;

public:
    using base_::base_;

    virtual ~consumer_module() override = default;

    virtual void init() override
    {
        this->base_::init();
        event_manager().connect<number_event>(*this);
        users().reserve(6);
    }

    void run_loop(appt::dt::seconds)
    {
        event_manager().emit(event_box());

        std::cout << "[ ";
        for (const auto& user_sptr : users())
            std::cout << user_sptr->name() << "  ";
        std::cout << " ]" << std::endl;
        if (users().size() >= 6)
            stop();
    }

    virtual void finish() override
    {
        std::cout << "consumer finished" << std::endl;
        app().stop_side_modules();
    }

    void receive(number_event& event)
    {
        if (users().size() < 6)
        {
            std::cout << "received: " << event.number << std::endl;
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
    generator_module(application_type& app) : base_(app, "first_module"), int_generator_(std::random_device{}()) {}
    virtual ~generator_module() override = default;

    void run_loop(appt::dt::seconds)
    {
        number_event event{ die100() };
        app().event_manager().emit(event);
    }

    virtual void finish() override { std::cout << "generator finished" << std::endl; }

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
    app.create_main_module<example::consumer_module>().set_frequency(60);
    app.create_module<example::generator_module>().set_frequency(40);
    app.init();
    auto app_result = app.run();
    std::cout << "TEST PACKAGE SUCCESS" << std::endl;
    return app_result;
}
