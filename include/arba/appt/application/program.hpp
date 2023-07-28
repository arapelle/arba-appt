#pragma once 

#include "program_args.hpp"
#include <string>

inline namespace arba
{
namespace appt
{

class program
{
public:
    template <typename other_application_type>
    using rebind_t = program;

    program(int argc, char** argv);
    explicit program(const program_args& args = program_args());

    inline const program_args& args() const { return program_args_; }

private:
    program_args program_args_;
};

}
}
