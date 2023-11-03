#include <arba/appt/application/application.hpp>
#include <arba/appt/application/decorator/multi_task.hpp>
#include <arba/appt/application/module/module.hpp>
#include <arba/appt/application/module/decorator/loop.hpp>
#include <random>
#include <iostream>

namespace example
{
class application : public appt::multi_task<appt::application<>, application>
{
private:
    using base_ = appt::multi_task<appt::application<>, application>;

public:
    using base_::base_;

    void init()
    {
        base_::init();
    }
};

struct number_event
{
    unsigned number;
};

using module = appt::module<application>;
template <class module_type>
using loop_module = appt::loop<module, module_type>;

class consumer_module : public loop_module<consumer_module>,
                        public evnt::event_listener<number_event>
{
private:
    using base_ = loop_module<consumer_module>;

public:
    using base_::base_;

    virtual ~consumer_module() override = default;

    virtual void init() override
    {
        this->base_::init();
        event_manager().connect<number_event>(*this);
        numbers_.reserve(6);
    }

    void run_loop(appt::dt::seconds)
    {
        event_manager().emit(event_box());

        std::cout << "[ ";
        for (unsigned n : numbers_)
            std::cout << n << " ";
        std::cout << " ]" << std::endl;
        if (numbers_.size() >= 6)
            stop();
    }

    virtual void finish() override
    {
        std::cout << "consumer finished" << std::endl;
        app().stop_side_modules();
    }

    void receive(number_event& event)
    {
        if (numbers_.size() < 6)
        {
            std::cout << "received: " << event.number << std::endl;
            numbers_.push_back(event.number);
        }
    }

private:
    std::vector<unsigned> numbers_;
};

class generator_module : public loop_module<generator_module>
{
private:
    using base_ = loop_module<generator_module>;

public:
    generator_module(std::string_view name = std::string_view())
        : base_(name), int_generator_(std::random_device{}())
    {}

    virtual ~generator_module() override = default;

    void run_loop(appt::dt::seconds)
    {
        number_event event{ die6() };
        app().event_manager().emit(event);
    }

    virtual void finish() override
    {
        std::cout << "generator finished" << std::endl;
    }

private:
    unsigned die6()
    {
        static std::uniform_int_distribution<> die(1, 6);
        return die(int_generator_);
    }

private:
    std::mt19937_64 int_generator_;
};

}

int main(int argc, char** argv)
{
    example::application app(appt::program_args(argc, argv));
    app.create_main_module<example::consumer_module>("consumer_module").set_frequency(3);
    app.create_module<example::generator_module>("generator_module").set_frequency(2);
    app.init();
    return app.run();
}
