#include <appt/application/module/module.hpp>

inline namespace arba
{
namespace appt
{

module_interface::module_interface()
{}

module_interface::jrunner module_interface::jthread_runner()
{
    if (!stop_token_.stop_possible()) [[likely]]
        return jrunner{this};
    throw std::runtime_error("jthread_runner() can be called only if the module is not already running.");
}

module_interface::jrunner::~jrunner()
{
    module_ptr->stop_token_ = std::stop_token();
}

void module_interface::jrunner::operator()(std::stop_token s_token)
{
    module_ptr->stop_token_ = s_token;
    module_ptr->run();
}

}
}
