#pragma once 

#include "program_args.hpp"
#include <string>

inline namespace arba
{
namespace appt
{

template <class ApplicationType = void>
class program;

template <>
class program<void>
{
public:
    template <typename OtherApplicationType>
    using rebind_t = program<OtherApplicationType>;

    program(int argc, char** argv);
    explicit program(const program_args& args = program_args());

    inline const program_args& args() const { return program_args_; }

private:
    program_args program_args_;
};

template <class ApplicationType>
class program : public program<void>
{
private:
    using base_ = program<void>;

public:
    using base_::base_;

protected:
    using self_type_ = ApplicationType;

    [[nodiscard]] inline const self_type_& self_() const noexcept { return static_cast<self_type_&>(*this); }
    [[nodiscard]] inline self_type_& self_() noexcept { return static_cast<self_type_&>(*this); }
};

}
}
