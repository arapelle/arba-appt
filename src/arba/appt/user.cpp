#include <arba/appt/user/user.hpp>

#include <stdexcept>

inline namespace arba
{
namespace appt
{

void user::set_id(user::id_type id)
{
    if (id_ != bad_id) [[unlikely]]
        throw std::runtime_error("This user has already an id!");
    id_ = id;
}

void user::unset_id()
{
    if (id_ == bad_id) [[unlikely]]
        throw std::runtime_error("This user has already a bad id!");
    id_ = bad_id;
}

} // namespace appt
} // namespace arba
