#pragma once

#include "user.hpp"
#include <memory>

inline namespace arba
{
namespace appt
{

template <typename UserType>
requires std::is_base_of_v<user, UserType>
struct user_sptr_id_hash
{
    using key_type = appt::user_id;
    using is_transparent = void;

    std::size_t operator()(const std::shared_ptr<UserType>& arg) const noexcept
    {
        return arg ? arg->id() : std::numeric_limits<std::size_t>::max();
    }

    std::size_t operator()(const key_type& arg) const noexcept
    {
        return std::hash<key_type>{}(arg);
    }

    inline static const appt::user_id& user_id(const UserType& arg) { return arg.id(); }
};

template <typename UserType>
struct user_sptr_name_hash;

template <typename UserType>
requires std::is_base_of_v<user, UserType>
&& requires(UserType const* user)
{
    { user->name() };
}
struct user_sptr_name_hash<UserType>
{
public:
    using name_type = std::remove_cvref_t<decltype(std::declval<UserType*>()->name())>;
    using key_type = name_type;
    using is_transparent = void;

    std::size_t operator()(const std::shared_ptr<UserType>& arg) const noexcept
    {
        return arg ? std::hash<name_type>{}(arg->name()) : 0;
    }

    std::size_t operator()(const name_type& arg) const noexcept
    {
        return std::hash<name_type>{}(arg);
    }

    inline static const name_type& user_id(const UserType& arg) { return arg.name(); }
};

template <typename UserType>
requires std::is_base_of_v<user, UserType>
&& requires(UserType const* user)
{
    { user->name };
}
struct user_sptr_name_hash<UserType>
{
public:
    using name_type = std::remove_cvref_t<decltype(std::declval<UserType*>()->name)>;
    using key_type = name_type;
    using is_transparent = void;

    std::size_t operator()(const std::shared_ptr<UserType>& arg) const noexcept
    {
        return arg ? std::hash<name_type>{}(arg->name) : 0;
    }

    std::size_t operator()(const name_type& arg) const noexcept
    {
        return std::hash<name_type>{}(arg);
    }

    inline static const name_type& user_id(const UserType& arg) { return arg.name; }
};

template <typename UserType, typename UserIdType, const UserIdType&(UserType::*GetUserId)() const>
struct user_sptr_mem_fn_hash
{
public:
    using key_type = UserIdType;
    using is_transparent = void;

    std::size_t operator()(const std::shared_ptr<UserType>& arg) const noexcept
    {
        return arg ? std::hash<key_type>{}(user_id(*arg)) : 0;
    }

    std::size_t operator()(const key_type& arg) const noexcept
    {
        return std::hash<key_type>{}(arg);
    }

    inline static const key_type& user_id(const UserType& arg) { return (arg.*GetUserId)(); }
};

template <typename UserType, typename UserIdType, UserIdType UserType::* UsrId>
struct user_sptr_mem_hash
{
public:
    using key_type = UserIdType;
    using is_transparent = void;

    std::size_t operator()(const std::shared_ptr<UserType>& arg) const noexcept
    {
        return arg ? std::hash<key_type>{}(user_id(*arg)) : 0;
    }

    std::size_t operator()(const key_type& arg) const noexcept
    {
        return std::hash<key_type>{}(arg);
    }

    inline static const key_type& user_id(const UserType& arg) { return arg.*UsrId; }
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
