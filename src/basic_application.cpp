#include <arba/appt/application/basic_application.hpp>

inline namespace arba
{
namespace appt
{

basic_application<>::basic_application(int argc, char** argv)
    : basic_application(program_args(argc, argv))
{}

basic_application<>::basic_application(const program_args &args)
    : program_args_(args)
{}

}
}
