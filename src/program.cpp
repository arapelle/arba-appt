#include <arba/appt/application/program.hpp>

inline namespace arba
{
namespace appt
{

program<>::program(int argc, char** argv)
    : program(program_args(argc, argv))
{}

program<>::program(const program_args &args)
    : program_args_(args)
{}

}
}
