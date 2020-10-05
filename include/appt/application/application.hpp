#pragma once 

#include "program_args.hpp"
#include <rsce/resource_manager.hpp>
#include <evnt/evnt.hpp>
#include <string>

inline namespace arba
{
namespace appt
{

class application
{
public:
    template <typename other_application_type>
    using rebind_t = application;

    application(int argc, char** argv);
    application(const program_args& args = program_args());

    inline const program_args& args() const { return program_args_; }
    inline const rsce::resource_manager& resource_manager() const { return resource_manager_; }
    inline       rsce::resource_manager& resource_manager() { return resource_manager_; }
    inline const evnt::event_manager& event_manager() const { return event_manager_; }
    inline       evnt::event_manager& event_manager() { return event_manager_; }
    inline const vlfs::virtual_filesystem& virtual_filesystem() const { return virtual_filesystem_; }
    inline       vlfs::virtual_filesystem& virtual_filesystem() { return virtual_filesystem_; }

private:
    program_args program_args_;
    rsce::resource_manager resource_manager_;
    vlfs::virtual_filesystem virtual_filesystem_;
    evnt::event_manager event_manager_;
};

}
}
