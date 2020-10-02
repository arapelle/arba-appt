#pragma once

#include "user.hpp"
#include <vector>
#include <memory>
#include <mutex>
#include <stdexcept>

inline namespace arba
{
namespace appt
{

template <class user_type>
requires std::is_base_of_v<user, user_type>
class user_manager
{
public:
    using id_type = typename user_type::id_type;

public:
    inline user_manager() {}
    template <typename... args_types>
    std::shared_ptr<user_type> create_user(args_types&&... args);
    std::shared_ptr<user_type> shared_user(const id_type& user_id);
    void release_user(const id_type& user_id);
    void reset_user_shared_ptr(std::shared_ptr<user_type>& user_sptr);

private:
    void release_user_(const id_type& usr_id, std::weak_ptr<user_type>& user_wptr);

private:
    std::vector<std::weak_ptr<user_type>> users_;
    integer_id_factory<id_type> user_id_factory_;
    std::mutex mutex_;
};

// Template methods implementation:

template <class user_type>
template <typename... args_types>
std::shared_ptr<user_type> user_manager<user_type>::create_user(args_types&&... args)
{
    std::lock_guard lock(mutex_);
    std::shared_ptr user_sptr = std::make_shared<user_type>(std::forward<args_types>(args)...);
    id_type id = user_id_factory_.new_id();
    user_sptr->set_id(id);
    if (id < users_.size())
        users_[id] = user_sptr;
    else
        users_.push_back(user_sptr);
    return user_sptr;
}

template <class user_type>
std::shared_ptr<user_type> user_manager<user_type>::shared_user(const id_type& user_id)
{
    std::lock_guard lock(mutex_);
    std::weak_ptr<user_type>& user_wptr = users_[user_id];
    if (!user_wptr.expired()) [[likely]]
        return user_wptr.lock();
    return std::shared_ptr<user_type>();
}

template <class user_type>
void user_manager<user_type>::release_user(const id_type& user_id)
{
    std::lock_guard lock(mutex_);
    std::weak_ptr<user_type>& user_wptr = users_[user_id];
    if (user_wptr.expired()) [[likely]]
        release_user_(user_id, user_wptr);
    else
        throw std::invalid_argument("The user cannot be removed as it is still used.");
}

template <class user_type>
void user_manager<user_type>::reset_user_shared_ptr(std::shared_ptr<user_type>& user_sptr)
{
    id_type usr_id = user_sptr->id();
    std::lock_guard lock(mutex_);
    std::weak_ptr<user_type>& user_wptr = users_[usr_id];
    user_sptr.reset();
    if (user_wptr.expired()) [[likely]]
        release_user_(usr_id, user_wptr);
}

template <class user_type>
void user_manager<user_type>::release_user_(const id_type& usr_id, std::weak_ptr<user_type>& user_wptr)
{
    user_id_factory_.delete_id(usr_id);
    user_wptr.reset();
    while ((!users_.empty()) && users_.back().expired())
        users_.pop_back();
}

}
}
