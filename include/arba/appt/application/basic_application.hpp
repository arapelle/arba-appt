#pragma once 

#include "program_args.hpp"
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
    template <typename OtherApplicationType>
    using rebind_t = basic_application<OtherApplicationType>;

    explicit basic_application(const program_args& args = program_args());

    inline const program_args& args() const { return program_args_; }

private:
    program_args program_args_;
};

template <class ApplicationType>
class basic_application : public basic_application<void>
{
private:
    using base_ = basic_application<void>;

public:
    using base_::base_;

protected:
    using self_type_ = ApplicationType;

    [[nodiscard]] inline const self_type_& self_() const noexcept { return static_cast<self_type_&>(*this); }
    [[nodiscard]] inline self_type_& self_() noexcept { return static_cast<self_type_&>(*this); }
};

}
}
