#pragma once

#include "user_manager.hpp"
#include "user_hash.hpp"
#include <unordered_set>

inline namespace arba
{
namespace appt
{

template <class user_type, class user_sptr_hash>
class user_set : private std::unordered_set<std::shared_ptr<user_type>, user_sptr_hash>
{
private:
    using base_ = std::unordered_set<std::shared_ptr<user_type>, user_sptr_hash>;

private:
    using user_type_sptr = std::shared_ptr<user_type>;
    using user_set_container = std::unordered_set<user_type_sptr, user_sptr_hash>;
    using key_type = typename user_sptr_hash::key_type;

public:
    inline void set_user_manager(user_manager<user_type>& usr_manager);

    using user_set_container::empty;
    using user_set_container::size;
    using user_set_container::begin;
    using user_set_container::end;
    using user_set_container::cbegin;
    using user_set_container::cend;
    using iterator = user_set_container::iterator;
    using const_iterator = user_set_container::const_iterator;

    user_set() = default;
    explicit user_set(const user_set&) = default;
    ~user_set() { clear_users(); }
    user_set& operator=(const user_set& other);
    user_set& operator=(user_set&& other);

    inline auto insert_user(const user_type_sptr& user_sptr);
    inline void erase_user(const_iterator iter) { this->erase(iter); }

    template <typename... args_types>
    std::shared_ptr<user_type> create_user(args_types&&... args);
    const_iterator find_user(const key_type& key) const;
    iterator find_user(const key_type& key);
    inline user_type_sptr find_user_sptr(const key_type& key) const;
    void erase_user(const key_type& key);
    void clear_users();

    std::size_t max_number_of_users() const { return max_number_of_users_; }
    void set_max_number_of_users(std::size_t max_count) { max_number_of_users_ = max_count; }

private:
    user_manager<user_type>* user_manager_ = nullptr;
    std::size_t max_number_of_users_ = std::numeric_limits<std::size_t>::max();
};

template <class user_type, class user_sptr_hash>
inline auto user_set<user_type, user_sptr_hash>::insert_user(const user_type_sptr& user_sptr)
{
    if (this->size() < max_number_of_users_)
        return this->insert(user_sptr);
    return std::make_pair(this->end(), false);
}

template <class user_type, class user_sptr_hash>
void user_set<user_type, user_sptr_hash>::set_user_manager(user_manager<user_type>& usr_manager)
{
    if (user_manager_)
        throw std::runtime_error("The user_set is already using another user_manager.");
    if (!empty())
        throw std::runtime_error("The user_set should be empty before link it to a user_manager.");
    user_manager_ = &usr_manager;
}

template <class user_type, class user_sptr_hash>
user_set<user_type, user_sptr_hash>&
user_set<user_type, user_sptr_hash>::operator=(const user_set &other)
{
    if (&other != this)
        clear_users();
    user_manager_ = other.user_manager_;
    static_cast<base_&>(*this) = static_cast<const base_&>(other);
    return *this;
}

template <class user_type, class user_sptr_hash>
user_set<user_type, user_sptr_hash>&
user_set<user_type, user_sptr_hash>::operator=(user_set&& other)
{
    if (&other != this)
        clear_users();
    user_manager_ = other.user_manager_;
    other.user_manager_ = nullptr;
    static_cast<base_&>(*this) = std::move(static_cast<base_&>(other));
    return *this;
}

template <class user_type, class user_sptr_hash>
user_set<user_type, user_sptr_hash>::const_iterator
        user_set<user_type, user_sptr_hash>::find_user(const key_type& key) const
{
    // when C++20 heterogenous lookup is ready:
    //    return this->find(name);
    // instead of:
    return std::find_if(begin(), end(),
                        [&](const user_type_sptr& arg)
                        { return user_sptr_hash::user_id(*arg) == key; });
}

template <class user_type, class user_sptr_hash>
user_set<user_type, user_sptr_hash>::iterator
user_set<user_type, user_sptr_hash>::find_user(const key_type& key)
{
    // when C++20 heterogenous lookup is ready:
//    return this->find(name);
    // instead of:
    return std::find_if(begin(), end(),
                        [&](const user_type_sptr& arg)
                        { return user_sptr_hash::user_id(*arg) == key; });
}

template <class user_type, class user_sptr_hash>
user_set<user_type, user_sptr_hash>::user_type_sptr
user_set<user_type, user_sptr_hash>::find_user_sptr(const user_set::key_type &key) const
{
    auto iter = find_user(key);
    return iter != end() ? *iter : nullptr;
}

template <class user_type, class user_sptr_hash>
void user_set<user_type, user_sptr_hash>::erase_user(const key_type& key)
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

template <class user_type, class user_sptr_hash>
void user_set<user_type, user_sptr_hash>::clear_users()
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

template <class user_type, class user_sptr_hash>
template <typename... args_types>
std::shared_ptr<user_type> user_set<user_type, user_sptr_hash>::create_user(args_types&&... args)
{
    if (this->size() < max_number_of_users_)
    {
        user_type_sptr user_sptr;
        if (user_manager_)
            user_sptr = user_manager_->create_user(std::forward<args_types>(args)...);
        else
            user_sptr = std::make_shared<user_type>(std::forward<args_types>(args)...);
        if (user_sptr)
            this->insert(user_sptr);
        return user_sptr;
    }
    return nullptr;
}

}
}
