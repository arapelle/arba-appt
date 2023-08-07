#include <arba/appt/application/module/module_interface.hpp>
#include <format>

inline namespace arba
{
namespace appt
{

module_interface::module_interface(std::string_view name)
    : name_(name.size() ? name : std::format("module_{}", new_module_index_()))
{}

}
}
