#pragma once

#include <cassert>
#include <queue>

inline namespace arba
{
namespace appt
{

template <class id_type_ = std::size_t>
class integer_id_factory
{
public:
    using id_type = id_type_;

private:
    using id_queue_type_ = std::priority_queue<id_type, std::vector<id_type>, std::greater<id_type>>;

public:
    explicit integer_id_factory(id_type first_value = id_type(0)) : first_value_(first_value), id_next_(first_value) {}

    id_type new_id()
    {
        if (id_queue_.empty())
            return id_next_++;

        id_type result = id_queue_.top();
        id_queue_.pop();
        return result;
    }

    void delete_id(id_type id)
    {
        assert(id < id_next_);

        if (id == (id_next_ - 1))
        {
            --id_next_;
            while (id_queue_.size() && id_queue_.top() == (id_next_ - 1))
            {
                --id_next_;
                id_queue_.pop();
            }
        }
        else
            id_queue_.push(id);
    }

    std::size_t number_of_valid_id() const { return id_next_ - first_value_ - id_queue_.size(); }

    void reset(id_type first_value = id_type(0))
    {
        first_value_ = first_value;
        id_next_ = first_value;
        id_queue_ = id_queue_type_();
    }

private:
    id_type first_value_;
    id_type id_next_;
    id_queue_type_ id_queue_;
};

} // namespace appt
} // namespace arba
