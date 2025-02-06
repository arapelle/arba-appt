#pragma once

#include <arba/core/program_args.hpp>

#include <string>

inline namespace arba
{
namespace appt
{

template <class ApplicationType = void>
class basic_application;

template <>
class basic_application<void>
{
public:
    template <typename OtherType>
    using rebind_t = basic_application<OtherType>;

    explicit basic_application(const core::program_args& args = core::program_args());

    inline const core::program_args& args() const { return program_args_; }

private:
    core::program_args program_args_;
};

template <class ApplicationType>
class basic_application : public basic_application<void>
{
private:
    using base_ = basic_application<void>;

public:
    using base_::base_;

protected:
    using self_type = ApplicationType;

    [[nodiscard]] inline const self_type& self() const noexcept { return static_cast<const self_type&>(*this); }
    [[nodiscard]] inline self_type& self() noexcept { return static_cast<self_type&>(*this); }
};

} // namespace appt
} // namespace arba
