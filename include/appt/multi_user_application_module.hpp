#pragma once

#include "multi_user_application.hpp"
#include "application_module.hpp"
#include "user_set.hpp"
#include <memory>

inline namespace arba
{
namespace appt
{

template <typename user_type, typename user_sptr_hash, typename application_module_base_type>
class multi_user_mod;

template <typename user_type, typename user_sptr_hash, typename application_module_base_type>
class multi_user_mod : public application_module_base_type
{
public:
    using user_set = appt::user_set<user_type, user_sptr_hash>;
    using application_type = typename application_module_base_type::application_type;

    multi_user_mod() {}
    explicit multi_user_mod(application_type& app);

    inline const user_set& users() const { return users_; }
    inline user_set& users() { return users_; }

    void set_app(application_type& app);

private:
    user_set users_;
};

template <typename user_type, typename user_sptr_hash, typename application_module_base_type>
multi_user_mod<user_type, user_sptr_hash, application_module_base_type>::multi_user_mod(multi_user_mod::application_type& app)
    : application_module_base_type(app)
{
    users_.set_user_manager(app.usr_manager());
}

template <typename user_type, typename user_sptr_hash, typename application_module_base_type>
void multi_user_mod<user_type, user_sptr_hash, application_module_base_type>::set_app(multi_user_mod::application_type &app)
{
    this->application_module_base_type::set_app(app);
    users_.set_user_manager(app.usr_manager());
}

//-------------------------------------------------------------------------------------------------

template <typename user_type, typename user_sptr_hash, typename app_type = application>
class multi_user_application_module : public multi_user_mod<user_type, user_sptr_hash, application_module<app_type>>
{
    using base_ = multi_user_mod<user_type, user_sptr_hash, application_module<app_type>>;

public:
    using base_::multi_user_mod;
};

}
}
