#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <arba/appt/application/application.hpp>
#include <arba/appt/application/decorator/logging.hpp>

namespace example
{
class logging_application : public appt::adec::logging<appt::application_logger, appt::application<>>
{
private:
    using base_ = appt::adec::logging<appt::application_logger, appt::application<>>;

public:
    using base_::base_;

    void init()
    {
        logger()->set_level(spdlog::level::trace);
    }

    void run()
    {
        SPDLOG_LOGGER_TRACE(logger(), __PRETTY_FUNCTION__);
        SPDLOG_LOGGER_INFO(logger(), "info");
    }
};

}

int main(int argc, char** argv)
{
    example::logging_application app(appt::program_args(argc, argv));
    app.init();
    app.run();
    SPDLOG_LOGGER_INFO(app.logger(), "EXIT SUCCESS");

    return EXIT_SUCCESS;
}
