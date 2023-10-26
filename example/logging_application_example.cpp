#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <arba/appt/application/application.hpp>
#include <arba/appt/application/decorator/logging.hpp>
#include <arba/appt/util/logging/logging_macro.hpp>
#include <spdlog/spdlog.h>
#include <source_location>
#include <iostream>

namespace example
{
class logging_application : public appt::adec::logging<appt::application<>, logging_application>
{
private:
    using base_ = appt::adec::logging<appt::application<>, logging_application>;

public:
    using base_::base_;

    void init()
    {
        logger()->set_level(spdlog::level::trace);
    }

    void run()
    {
        log_trace_arg<int>();
        SPDLOG_LOGGER_INFO(logger(), "info");
    }

    template <class Arg>
    void log_trace_arg()
    {
        ARBA_APPT_LOGGER_TRACE(logger());
    }
};

}

int main(int argc, char** argv)
{
    std::filesystem::path app_log_path;
    {
        example::logging_application app(appt::program_args(argc, argv));
        app_log_path = app.log_path();
        app.init();
        app.run();
        SPDLOG_LOGGER_INFO(app.logger(), "EXIT SUCCESS");
    }
    std::cout << "Application file log:" << std::endl;
    std::ifstream stream(app_log_path);
    for (std::string line; stream;)
    {
        std::getline(stream, line);
        std::cout << line << std::endl;
    }

    return EXIT_SUCCESS;
}
