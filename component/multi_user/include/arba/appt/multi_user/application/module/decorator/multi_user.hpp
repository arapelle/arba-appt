#pragma once

#include <arba/appt/application/module/basic_module.hpp>
#include <arba/appt/multi_user/user/user_set.hpp>

#include <memory>

inline namespace arba
{
namespace appt
{
inline namespace mdec // module_decorator
{

template <typename UserType, typename UserSptrHash, typename ModuleBase, typename SelfType = void>
class multi_user;

template <typename UserType, typename UserSptrHash, typename ModuleBase>
class multi_user<UserType, UserSptrHash, ModuleBase> : public ModuleBase
{
public:
    using user_set = appt::user_set<UserType, UserSptrHash>;
    using application_type = typename ModuleBase::application_type;

    template <typename OtherType>
    using rebind_t = multi_user<UserType, UserSptrHash, ModuleBase, OtherType>;

    explicit multi_user(application_type& app, std::string_view name = std::string_view()) : ModuleBase(app, name)
    {
        users_.set_user_manager(app.usr_manager());
    }
    virtual ~multi_user() override = default;

    inline const user_set& users() const { return users_; }
    inline user_set& users() { return users_; }

private:
    user_set users_;
};

template <typename UserType, typename UserSptrHash, typename ModuleBase, typename SelfType>
class multi_user : public multi_user<UserType, UserSptrHash, typename ModuleBase::template rebind_t<SelfType>>
{
private:
    using base_ = multi_user<UserType, UserSptrHash, typename ModuleBase::template rebind_t<SelfType>>;

public:
    using base_::base_;
    virtual ~multi_user() override = default;
};

} // namespace mdec
} // namespace appt
} // namespace arba
