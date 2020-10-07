#pragma once

#include <appt/util/console_file_logger.hpp>

inline namespace arba
{
namespace appt
{

class application_logger : public console_file_logger
{
public:
    template <class app_type>
    inline explicit application_logger(const app_type& app)
        : console_file_logger(logger_name(app.args().program_stem().generic_string()),
                              log_file(app.log_dir(), app.args().program_stem().generic_string()))
    {}

protected:
    inline static std::string logger_name(std::string program_stem)
    {
        return program_stem.empty() ? "application" : program_stem;
    }

    inline static std::filesystem::path log_file(std::filesystem::path log_dir, std::string program_stem)
    {
        if (!program_stem.empty())
            return log_dir/(program_stem + ".log");
        return log_dir/"application.log";
    }
};

}
}
