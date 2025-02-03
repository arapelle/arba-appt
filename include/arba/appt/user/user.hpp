#pragma once

#include <arba/appt/util/integer_id_factory.hpp>
#include <limits>

inline namespace arba
{
namespace appt
{

class user
{
public:
    using id_type = std::size_t;

public:
    inline static constexpr id_type bad_id = std::numeric_limits<id_type>::max();

public:
    user() : id_(bad_id) {}
    inline const id_type& id() const { return id_; }
    inline bool operator==(const user& rhs) const = default;
    inline bool operator!=(const user& rhs) const = default;

private:
    template <class UserType>
    requires std::is_base_of_v<user, UserType>
    friend class user_manager;
    void set_id(id_type id);
    void unset_id();

private:
    id_type id_;
};

using user_id = user::id_type;

}
}
