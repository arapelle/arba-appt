#include <appt/application.hpp>

inline namespace arba
{
namespace appt
{

application::application(int argc, char** argv)
    : program_args_(argc, argv), resource_manager_(virtual_filesystem_)
{}

}
}
