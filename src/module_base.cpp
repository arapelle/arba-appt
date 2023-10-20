#include <arba/appt/application/module/module_base.hpp>
#include <format>

inline namespace arba
{
namespace appt
{

module_base::module_base(std::string_view name)
    : name_(name.size() ? name : std::format("module_{}", new_module_index_()))
{}

}
}
