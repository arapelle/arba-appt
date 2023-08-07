#pragma once

#include <thread>

inline namespace arba
{
namespace appt
{

class module_interface
{
public:
    explicit module_interface(std::string_view name = std::string_view());
    virtual ~module_interface() = default;

    inline const std::string& name() const { return name_; }
    virtual void init() {}
    virtual void run() = 0;
    virtual void stop() {}

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
