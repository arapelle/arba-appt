#include <arba/appt/application/decorator/multi_task.hpp>
#include <gtest/gtest.h>
#include <cstdlib>

using namespace std::string_literals;

std::array s_args = { "/root/dir/program_name.v2.run"s, "6"s, "-c"s, "Debug"s };
std::array cs_args = { s_args[0].data(), s_args[1].data(), s_args[2].data(), s_args[3].data() };
int argc = cs_args.size();
char** argv = cs_args.data();

class ut_application : public appt::adec::multi_task<appt::application, ut_application>
{
    using base_ = appt::adec::multi_task<appt::application, ut_application>;

public:
    using base_::base_;

    void init()
    {
        if (event_manager().max_number_of_event_types() == 0)
            event_manager().resize(1);
        this->base_::init();
    }
};

class ut_event
{
public:
    std::string message;
};

class ut_first_event_module : public appt::module<ut_application>, public evnt::event_listener<ut_event>
{
    using base_ = appt::module<ut_application>;

public:
    virtual ~ut_first_event_module() override = default;

    virtual void init() override
    {
        this->base_::init();
        event_manager().connect<ut_event>(*this);
    }

    virtual void run() override
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ++run_count;
        event_manager().emit(event_box());
        if (run_count == 1)
        {
            event_manager().emit(ut_event{"local"});
            app().event_manager().emit(ut_event{"global"});
        }
    }

    void receive(ut_event& event)
    {
        messages.push_back(event.message);
    }

    uint16_t run_count = 0;
    std::vector<std::string> messages;
};

class ut_second_event_module : public appt::module<ut_application>, public evnt::event_listener<ut_event>
{
    using base_ = appt::module<ut_application>;

public:
    virtual ~ut_second_event_module() override = default;

    virtual void init() override
    {
        this->base_::init();
        event_manager().connect<ut_event>(*this);
    }

    virtual void run() override
    {
        ++run_count;
        event_manager().emit(event_box());
    }

    void receive(ut_event& event)
    {
        messages.push_back(event.message);
    }

    uint16_t run_count = 0;
    std::vector<std::string> messages;
};

TEST(event_forwarding_tests, test_forwarding)
{
    ut_application app(argc, argv);
    ut_first_event_module& first_module = app.create_module<ut_first_event_module>();
    ut_second_event_module& second_module = app.create_module<ut_second_event_module>();
    app.init();
    app.run();
    ASSERT_EQ(first_module.run_count, 1);
    ASSERT_EQ(second_module.run_count, 1);
    ASSERT_EQ(first_module.messages.size(), 1);
    ASSERT_EQ(first_module.messages.front(), "local");
    ASSERT_EQ(second_module.messages.size(), 0);
    app.run();
    ASSERT_EQ(first_module.run_count, 2);
    ASSERT_EQ(second_module.run_count, 2);
    ASSERT_EQ(first_module.messages.size(), 2);
    ASSERT_EQ(first_module.messages.front(), "local");
    ASSERT_EQ(first_module.messages.back(), "global");
    ASSERT_EQ(second_module.messages.size(), 1);
    ASSERT_EQ(second_module.messages.front(), "global");
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
