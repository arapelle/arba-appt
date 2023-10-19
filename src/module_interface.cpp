#include <arba/appt/application/module/module_interface.hpp>
#include <format>

inline namespace arba
{
namespace appt
{

module_interface::module_interface(std::string_view name)
    : name_(name.size() ? name : std::format("module_{}", new_module_index_()))
{}

void module_interface::run_maythrow()
{
    run_status_ = execution_status::executing;
    core::sbrm set_execution_failure_if_err = [this]{ run_status_ = execution_status::execution_failure; };
    this->run();
    set_execution_failure_if_err.disable();
    run_status_ = execution_status::execution_success;
}

}
}
