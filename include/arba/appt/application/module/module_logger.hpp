#pragma once

#include <arba/appt/util/console_file_logger.hpp>

inline namespace arba
{
namespace appt
{

class module_logger : public appt::console_file_logger
{
public:
    template <class module_type>
    inline explicit module_logger(const module_type& mod)
        : appt::console_file_logger(mod.name(), mod.app().log_directory()/(mod.name() + ".log"))
    {}
};

}
}
