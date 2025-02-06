#pragma once

#include "user.hpp"

#include <memory>
#include <mutex>
#include <stdexcept>
#include <vector>

inline namespace arba
{
namespace appt
{

template <class UserType>
    requires std::is_base_of_v<user, UserType>
class user_manager
{
public:
    using id_type = typename UserType::id_type;

public:
    inline user_manager() {}
    template <typename... ArgsTypes>
    std::shared_ptr<UserType> create_user(ArgsTypes&&... args);
    std::shared_ptr<UserType> shared_user(const id_type& user_id);
    void release_user(const id_type& user_id);
    void reset_user_shared_ptr(std::shared_ptr<UserType>& user_sptr);
    std::size_t size() const
    {
        std::lock_guard lock(mutex_);
        return user_id_factory_.number_of_valid_id();
    }
    std::size_t max_number_of_users() const
    {
        std::lock_guard lock(mutex_);
        return max_number_of_users_;
    }
    void set_max_number_of_users(std::size_t max_count)
    {
        std::lock_guard lock(mutex_);
        max_number_of_users_ = max_count;
    }

private:
    void release_user_(const id_type& usr_id, std::weak_ptr<UserType>& user_wptr);

private:
    std::vector<std::weak_ptr<UserType>> users_;
    integer_id_factory<id_type> user_id_factory_;
    std::size_t max_number_of_users_ = std::numeric_limits<std::size_t>::max();
    mutable std::mutex mutex_;
};

// Template methods implementation:

template <class UserType>
    requires std::is_base_of_v<user, UserType>
template <typename... ArgsTypes>
std::shared_ptr<UserType> user_manager<UserType>::create_user(ArgsTypes&&... args)
{
    std::lock_guard lock(mutex_);
    if (user_id_factory_.number_of_valid_id() == max_number_of_users_)
        return nullptr;
    std::shared_ptr user_sptr = std::make_shared<UserType>(std::forward<ArgsTypes>(args)...);
    id_type id = user_id_factory_.new_id();
    user_sptr->set_id(id);
    if (id < users_.size())
        users_[id] = user_sptr;
    else
        users_.push_back(user_sptr);
    return user_sptr;
}

template <class UserType>
    requires std::is_base_of_v<user, UserType>
std::shared_ptr<UserType> user_manager<UserType>::shared_user(const id_type& user_id)
{
    std::lock_guard lock(mutex_);
    std::weak_ptr<UserType>& user_wptr = users_[user_id];
    if (!user_wptr.expired()) [[likely]]
        return user_wptr.lock();
    return std::shared_ptr<UserType>();
}

template <class UserType>
    requires std::is_base_of_v<user, UserType>
void user_manager<UserType>::release_user(const id_type& user_id)
{
    std::lock_guard lock(mutex_);
    std::weak_ptr<UserType>& user_wptr = users_[user_id];
    if (user_wptr.expired()) [[likely]]
        release_user_(user_id, user_wptr);
    else
        throw std::invalid_argument("The user cannot be removed as it is still used.");
}

template <class UserType>
    requires std::is_base_of_v<user, UserType>
void user_manager<UserType>::reset_user_shared_ptr(std::shared_ptr<UserType>& user_sptr)
{
    id_type usr_id = user_sptr->id();
    std::lock_guard lock(mutex_);
    std::weak_ptr<UserType>& user_wptr = users_[usr_id];
    user_sptr.reset();
    if (user_wptr.expired()) [[likely]]
        release_user_(usr_id, user_wptr);
}

template <class UserType>
    requires std::is_base_of_v<user, UserType>
void user_manager<UserType>::release_user_(const id_type& usr_id, std::weak_ptr<UserType>& user_wptr)
{
    user_id_factory_.delete_id(usr_id);
    user_wptr.reset();
    while ((!users_.empty()) && users_.back().expired())
        users_.pop_back();
}

} // namespace appt
} // namespace arba
