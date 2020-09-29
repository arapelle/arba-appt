#pragma once

#include "util/integer_id_factory.hpp"

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
    void set_id(id_type id);
    void unset_id();
    inline bool operator==(const user& rhs) const = default;
    inline bool operator!=(const user& rhs) const = default;

private:
    id_type id_;
};

}
}
