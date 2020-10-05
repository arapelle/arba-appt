#include <appt/application/application.hpp>

inline namespace arba
{
namespace appt
{

application::application(int argc, char** argv)
    : application(program_args(argc, argv))
{}

application::application(const program_args &args)
    : program_args_(args), resource_manager_(virtual_filesystem_)
{}

}
}
