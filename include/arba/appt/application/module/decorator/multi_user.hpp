#pragma once

#include <arba/appt/application/module/module.hpp>
#include <arba/appt/user/user_set.hpp>
#include <memory>

inline namespace arba
{
namespace appt::mdec // module_decorator
{

template <typename user_type, typename user_sptr_hash, typename module_base_type, typename module_type = void>
class multi_user;

template <typename user_type, typename user_sptr_hash, typename module_base_type>
class multi_user<user_type, user_sptr_hash, module_base_type> : public module_base_type
{
public:
    using user_set = appt::user_set<user_type, user_sptr_hash>;
    using application_type = typename module_base_type::application_type;

    template <typename other_module_type>
    using rebind_t = multi_user<user_type, user_sptr_hash, module_base_type, other_module_type>;

    multi_user() {}
    explicit multi_user(std::string name) : module_base_type(std::move(name)) {}
    explicit multi_user(application_type& app);
    multi_user(std::string name, application_type& app);
    virtual ~multi_user() override = default;

    inline const user_set& users() const { return users_; }
    inline user_set& users() { return users_; }

    void set_app(application_type& app);

private:
    user_set users_;
};

template <typename user_type, typename user_sptr_hash, typename module_base_type>
multi_user<user_type, user_sptr_hash, module_base_type>::multi_user(multi_user::application_type& app)
    : module_base_type(app)
{
    users_.set_user_manager(app.usr_manager());
}

template <typename user_type, typename user_sptr_hash, typename module_base_type>
multi_user<user_type, user_sptr_hash, module_base_type>::multi_user(std::string name, multi_user::application_type &app)
    : module_base_type(std::move(name), app)
{
    users_.set_user_manager(app.usr_manager());
}

template <typename user_type, typename user_sptr_hash, typename module_base_type>
void multi_user<user_type, user_sptr_hash, module_base_type>::set_app(multi_user::application_type &app)
{
    this->module_base_type::set_app(app);
    users_.set_user_manager(app.usr_manager());
}

template <typename user_type, typename user_sptr_hash, typename module_base_type, typename module_type>
class multi_user : public multi_user<user_type, user_sptr_hash, typename module_base_type::rebind_t<module_type>>
{
private:
    using base_ = multi_user<user_type, user_sptr_hash, typename module_base_type::rebind_t<module_type>>;

public:
    using base_::base_;
    virtual ~multi_user() override = default;
};

}
}
