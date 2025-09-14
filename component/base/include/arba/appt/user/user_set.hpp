#pragma once

#include "user_hash.hpp"
#include "user_manager.hpp"

#include <algorithm>
#include <unordered_set>

inline namespace arba
{
namespace appt
{
namespace priv
{
template <typename UserType, typename UserSptrHash>
struct user_sptr_equal_to
{
    using is_transparent = void;

    constexpr bool operator()(const std::shared_ptr<UserType>& lhs, const std::shared_ptr<UserType>& rhs) const
    {
        if (lhs && rhs)
            return UserSptrHash::user_id(*lhs) == UserSptrHash::user_id(*rhs);
        return !lhs && !rhs;
    }

    constexpr bool operator()(const typename UserSptrHash::key_type& lhs, const std::shared_ptr<UserType>& rhs) const
    {
        return rhs && UserSptrHash::user_id(*rhs) == lhs;
    }
};
} // namespace priv

template <class UserType, class UserSptrHash>
class user_set : private std::unordered_set<std::shared_ptr<UserType>, UserSptrHash,
                                            priv::user_sptr_equal_to<UserType, UserSptrHash>>
{
private:
    using base_ =
        std::unordered_set<std::shared_ptr<UserType>, UserSptrHash, priv::user_sptr_equal_to<UserType, UserSptrHash>>;

private:
    using UserType_sptr = std::shared_ptr<UserType>;
    using user_set_container = base_;
    using key_type = typename UserSptrHash::key_type;

public:
    inline void set_user_manager(user_manager<UserType>& usr_manager);

    using user_set_container::begin;
    using user_set_container::cbegin;
    using user_set_container::cend;
    using user_set_container::empty;
    using user_set_container::end;
    using user_set_container::reserve;
    using user_set_container::size;
    using iterator = user_set_container::iterator;
    using const_iterator = user_set_container::const_iterator;

    user_set() = default;
    explicit user_set(const user_set&) = default;
    ~user_set() { clear_users(); }
    user_set& operator=(const user_set& other);
    user_set& operator=(user_set&& other);

    inline auto insert_user(const UserType_sptr& user_sptr);
    inline void erase_user(const_iterator iter) { this->erase(iter); }

    template <typename... ArgsTypes>
    std::shared_ptr<UserType> create_user(ArgsTypes&&... args);
    inline const_iterator find_user(const key_type& key) const { return this->find(key); }
    inline iterator find_user(const key_type& key) { return this->find(key); }
    inline UserType_sptr find_user_sptr(const key_type& key) const;
    void erase_user(const key_type& key);
    void clear_users();

    std::size_t max_number_of_users() const { return max_number_of_users_; }
    void set_max_number_of_users(std::size_t max_count) { max_number_of_users_ = max_count; }

private:
    user_manager<UserType>* user_manager_ = nullptr;
    std::size_t max_number_of_users_ = std::numeric_limits<std::size_t>::max();
};

template <class UserType, class UserSptrHash>
inline auto user_set<UserType, UserSptrHash>::insert_user(const UserType_sptr& user_sptr)
{
    if (this->size() < max_number_of_users_)
        return this->insert(user_sptr);
    return std::make_pair(this->end(), false);
}

template <class UserType, class UserSptrHash>
void user_set<UserType, UserSptrHash>::set_user_manager(user_manager<UserType>& usr_manager)
{
    if (user_manager_)
        throw std::runtime_error("The user_set is already using another user_manager.");
    if (!empty())
        throw std::runtime_error("The user_set should be empty before link it to a user_manager.");
    user_manager_ = &usr_manager;
}

template <class UserType, class UserSptrHash>
user_set<UserType, UserSptrHash>& user_set<UserType, UserSptrHash>::operator=(const user_set& other)
{
    if (&other != this)
        clear_users();
    user_manager_ = other.user_manager_;
    static_cast<base_&>(*this) = static_cast<const base_&>(other);
    return *this;
}

template <class UserType, class UserSptrHash>
user_set<UserType, UserSptrHash>& user_set<UserType, UserSptrHash>::operator=(user_set&& other)
{
    if (&other != this)
        clear_users();
    user_manager_ = other.user_manager_;
    other.user_manager_ = nullptr;
    static_cast<base_&>(*this) = std::move(static_cast<base_&>(other));
    return *this;
}

template <class UserType, class UserSptrHash>
user_set<UserType, UserSptrHash>::UserType_sptr
user_set<UserType, UserSptrHash>::find_user_sptr(const user_set::key_type& key) const
{
    auto iter = find_user(key);
    return iter != end() ? *iter : nullptr;
}

template <class UserType, class UserSptrHash>
void user_set<UserType, UserSptrHash>::erase_user(const key_type& key)
{
    auto iter = find_user(key);
    if (iter != end())
    {
        std::shared_ptr user_sptr = *iter;
        this->erase(iter);
        if (user_manager_)
            user_manager_->reset_user_shared_ptr(user_sptr);
    }
}

template <class UserType, class UserSptrHash>
void user_set<UserType, UserSptrHash>::clear_users()
{
    if (user_manager_)
    {
        while (!empty())
        {
            auto begin_iter = begin();
            std::shared_ptr user_sptr = *begin_iter;
            this->erase(begin_iter);
            user_manager_->reset_user_shared_ptr(user_sptr);
        }
    }
    else
        this->clear();
}

template <class UserType, class UserSptrHash>
template <typename... ArgsTypes>
std::shared_ptr<UserType> user_set<UserType, UserSptrHash>::create_user(ArgsTypes&&... args)
{
    if (this->size() < max_number_of_users_)
    {
        UserType_sptr user_sptr;
        if (user_manager_)
        {
            user_sptr = user_manager_->create_user(std::forward<ArgsTypes>(args)...);
            if (user_sptr)
            {
                if (this->insert(user_sptr).second)
                    return user_sptr;
                user_manager_->reset_user_shared_ptr(user_sptr);
            }
        }
        else
        {
            user_sptr = std::make_shared<UserType>(std::forward<ArgsTypes>(args)...);
            if (user_sptr && this->insert(user_sptr).second)
                return user_sptr;
        }
    }
    return nullptr;
}

} // namespace appt
} // namespace arba
