#pragma once

#include <arba/appt/application/program_args.hpp>
#include <arba/core/version/trinum_version.hpp>

inline namespace arba
{
namespace appt
{
inline namespace adec // application_decorator
{

template <class ApplicationBaseType, typename ApplicationType = void>
class trinum_versioned;

template <class ApplicationBaseType>
class trinum_versioned<ApplicationBaseType> : public ApplicationBaseType
{
public:
    template <typename other_ApplicationType>
    using rebind_t = trinum_versioned<ApplicationBaseType, other_ApplicationType>;

    using typename ApplicationBaseType::self_type;

    explicit trinum_versioned(const program_args& args = program_args())
        : ApplicationBaseType(args)
    {}

    [[nodiscard]] inline static constexpr auto trinum_version()
    {
        /*static constexpr core::trinum_version version(self_type::make_trinum_version());
        return version;*/
        return self_type::make_trinum_version();
    }

    [[nodiscard]] inline static constexpr auto version() { return trinum_version(); }

protected:
    [[nodiscard]] inline static constexpr auto make_trinum_version()
    {
        return core::trinum_version<0, 1, 0>{};
    }
};

template <class ApplicationBaseType, typename ApplicationType>
class trinum_versioned : public trinum_versioned<typename ApplicationBaseType::template rebind_t<ApplicationType>>
{
private:
    using base_ = trinum_versioned<typename ApplicationBaseType::template rebind_t<ApplicationType>>;

public:
    using base_::base_;
};

}
}
}
