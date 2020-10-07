#pragma once

#include <appt/application/module/multi_user_module.hpp>
#include <appt/user/user_set.hpp>
#include <memory>

inline namespace arba
{
namespace appt::mdec // module_decorator
{

template <typename user_type, typename user_sptr_hash, typename module_base_type>
class multi_user;

template <typename user_type, typename user_sptr_hash, typename module_base_type>
class multi_user : public module_base_type
{
public:
    using user_set = appt::user_set<user_type, user_sptr_hash>;
    using application_type = typename module_base_type::application_type;

    multi_user() {}
    explicit multi_user(std::string name) : module_base_type(std::move(name)) {}
    explicit multi_user(application_type& app);
    multi_user(std::string name, application_type& app);

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

}
}
