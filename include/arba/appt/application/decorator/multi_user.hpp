#pragma once

#include <arba/appt/user/user_manager.hpp>

#include <memory>

inline namespace arba
{
namespace appt
{
inline namespace adec // application_decorator
{

template <typename UserType, typename ApplicationBase, typename SelfType = void>
class multi_user;

template <typename UserType, typename ApplicationBase>
class multi_user<UserType, ApplicationBase> : public ApplicationBase
{
private:
    using user_sptr = std::shared_ptr<UserType>;
    using user_manager_type = user_manager<UserType>;

public:
    template <typename OtherType>
    using rebind_t = multi_user<UserType, ApplicationBase, OtherType>;

    using ApplicationBase::ApplicationBase;

    inline const user_manager_type& usr_manager() const { return user_manager_; }
    inline user_manager_type& usr_manager() { return user_manager_; }

private:
    user_manager_type user_manager_;
};

template <typename UserType, typename ApplicationBase, typename SelfType>
class multi_user : public multi_user<UserType, typename ApplicationBase::template rebind_t<SelfType>>
{
    using base_ = multi_user<UserType, typename ApplicationBase::template rebind_t<SelfType>>;

public:
    using base_::base_;
};

} // namespace adec
} // namespace appt
} // namespace arba
