#pragma once 

#include <appt/application/program_args.hpp>
#include <rsce/resource_manager.hpp>
#include <evnt/evnt.hpp>
#include <string>

inline namespace arba
{
namespace appt::adec // application_decorator
{

template <class application_type>
class toolkit : public application_type
{
public:
    template <typename other_application_type>
    using rebind_t = toolkit;

    toolkit(int argc, char** argv) : toolkit(program_args(argc, argv)) {}
    toolkit(const program_args& args = program_args()) : application_type(args), resource_manager_(virtual_filesystem_) {}

    inline const rsce::resource_manager& resource_manager() const { return resource_manager_; }
    inline       rsce::resource_manager& resource_manager() { return resource_manager_; }
    inline const evnt::event_manager& event_manager() const { return event_manager_; }
    inline       evnt::event_manager& event_manager() { return event_manager_; }
    inline const vlfs::virtual_filesystem& virtual_filesystem() const { return virtual_filesystem_; }
    inline       vlfs::virtual_filesystem& virtual_filesystem() { return virtual_filesystem_; }

private:
    rsce::resource_manager resource_manager_;
    vlfs::virtual_filesystem virtual_filesystem_;
    evnt::event_manager event_manager_;
};

}
}
