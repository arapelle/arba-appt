#pragma once

#include <arba/core/program_args.hpp>
#include <arba/evnt/evnt.hpp>
#include <arba/rsce/resource_manager.hpp>

#include <string>

inline namespace arba
{
namespace appt
{
inline namespace adec // application_decorator
{

template <class ApplicationBase, typename SelfType = void>
class toolkit;

template <class ApplicationBase>
class toolkit<ApplicationBase> : public ApplicationBase
{
public:
    template <typename OtherType>
    using rebind_t = toolkit<ApplicationBase, OtherType>;

    explicit toolkit(const core::program_args& args = core::program_args())
        : ApplicationBase(args), resource_manager_(virtual_filesystem_)
    {
        if (!args.empty())
            virtual_filesystem_.set_program_dir_virtual_root(args.program_dir());
    }

    inline const rsce::resource_manager& resource_manager() const { return resource_manager_; }
    inline rsce::resource_manager& resource_manager() { return resource_manager_; }
    inline const evnt::event_manager& event_manager() const { return event_manager_; }
    inline evnt::event_manager& event_manager() { return event_manager_; }
    inline const vlfs::virtual_filesystem& virtual_filesystem() const { return virtual_filesystem_; }
    inline vlfs::virtual_filesystem& virtual_filesystem() { return virtual_filesystem_; }

private:
    vlfs::virtual_filesystem virtual_filesystem_;
    rsce::resource_manager resource_manager_;
    evnt::event_manager event_manager_;
};

template <class ApplicationBase, typename SelfType>
class toolkit : public toolkit<typename ApplicationBase::template rebind_t<SelfType>>
{
private:
    using base_ = toolkit<typename ApplicationBase::template rebind_t<SelfType>>;

public:
    using base_::base_;
};

} // namespace adec
} // namespace appt
} // namespace arba
