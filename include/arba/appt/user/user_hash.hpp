#pragma once

#include "user.hpp"
#include <memory>

inline namespace arba
{
namespace appt
{

template <typename user_type>
requires std::is_base_of_v<user, user_type>
struct user_sptr_id_hash
{
    using key_type = appt::user_id;
    using is_transparent = void;

    std::size_t operator()(const std::shared_ptr<user_type>& arg) const noexcept
    {
        return arg ? arg->id() : std::numeric_limits<std::size_t>::max();
    }

    std::size_t operator()(const key_type& arg) const noexcept
    {
        return std::hash<key_type>{}(arg);
    }

    inline static const appt::user_id& user_id(const user_type& arg) { return arg.id(); }
};

template <typename user_type>
struct user_sptr_name_hash;

template <typename user_type>
requires std::is_base_of_v<user, user_type>
&& requires(user_type const* user)
{
    { user->name() };
}
struct user_sptr_name_hash<user_type>
{
public:
    using name_type = std::remove_cvref_t<decltype(std::declval<user_type*>()->name())>;
    using key_type = name_type;
    using is_transparent = void;

    std::size_t operator()(const std::shared_ptr<user_type>& arg) const noexcept
    {
        return arg ? std::hash<name_type>{}(arg->name()) : 0;
    }

    std::size_t operator()(const name_type& arg) const noexcept
    {
        return std::hash<name_type>{}(arg);
    }

    inline static const name_type& user_id(const user_type& arg) { return arg.name(); }
};

template <typename user_type>
requires std::is_base_of_v<user, user_type>
&& requires(user_type const* user)
{
    { user->name };
}
struct user_sptr_name_hash<user_type>
{
public:
    using name_type = std::remove_cvref_t<decltype(std::declval<user_type*>()->name)>;
    using key_type = name_type;
    using is_transparent = void;

    std::size_t operator()(const std::shared_ptr<user_type>& arg) const noexcept
    {
        return arg ? std::hash<name_type>{}(arg->name) : 0;
    }

    std::size_t operator()(const name_type& arg) const noexcept
    {
        return std::hash<name_type>{}(arg);
    }

    inline static const name_type& user_id(const user_type& arg) { return arg.name; }
};

template <typename user_type, typename user_id_type, const user_id_type&(user_type::*get_user_id)() const>
struct user_sptr_mem_fn_hash
{
public:
    using key_type = user_id_type;
    using is_transparent = void;

    std::size_t operator()(const std::shared_ptr<user_type>& arg) const noexcept
    {
        return arg ? std::hash<key_type>{}(user_id(*arg)) : 0;
    }

    std::size_t operator()(const key_type& arg) const noexcept
    {
        return std::hash<key_type>{}(arg);
    }

    inline static const key_type& user_id(const user_type& arg) { return (arg.*get_user_id)(); }
};

template <typename user_type, typename user_id_type, user_id_type user_type::* usr_id>
struct user_sptr_mem_hash
{
public:
    using key_type = user_id_type;
    using is_transparent = void;

    std::size_t operator()(const std::shared_ptr<user_type>& arg) const noexcept
    {
        return arg ? std::hash<key_type>{}(user_id(*arg)) : 0;
    }

    std::size_t operator()(const key_type& arg) const noexcept
    {
        return std::hash<key_type>{}(arg);
    }

    inline static const key_type& user_id(const user_type& arg) { return arg.*usr_id; }
};

}
}

namespace std
{

template <>
struct hash<arba::appt::user>
{
    std::size_t operator()(const arba::appt::user& arg) const noexcept
    {
        return arg.id();
    }
};

}
