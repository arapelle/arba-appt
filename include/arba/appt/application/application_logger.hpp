#pragma once

#include <arba/appt/util/console_file_logger.hpp>
#include "program_args.hpp"

inline namespace arba
{
namespace appt
{

class application_logger : public console_file_logger
{
public:
    template <class app_type>
    inline explicit application_logger(const app_type& app)
        : console_file_logger(logger_name(app.args()),
                              log_file(app.log_dir(), app.args()))
    {}

protected:
    inline static std::string logger_name(const program_args& args)
    {
        return args.empty() ? "application" : args.program_stem().generic_string();
    }

    inline static std::filesystem::path log_file(std::filesystem::path log_dir, const program_args& args)
    {
        return log_dir / (logger_name(args) + ".log");
    }
};

}
}
