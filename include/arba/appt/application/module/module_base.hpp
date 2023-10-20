#pragma once

#include <string>

inline namespace arba
{
namespace appt
{

class module_base
{
protected:
    explicit module_base(std::string_view name = std::string_view());

public:
    virtual ~module_base() = default;

    inline const std::string& name() const { return name_; }

private:
    std::size_t new_module_index_()
    {
        static std::size_t index = 0;
        return index++;
    }

private:
    std::string name_;
};

}
}
