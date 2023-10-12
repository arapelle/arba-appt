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

    const std::string& name() const { return name_; }
    virtual void init() {}
    virtual void run() = 0;

public:
    struct jrunner
    {
        module_interface*const module_ptr;
        ~jrunner();
        void operator()(std::stop_token s_token);
    };
    jrunner jthread_runner();

protected:
    inline const std::stop_token& stop_token() const { return stop_token_; }
    inline std::stop_token& stop_token() { return stop_token_; }

private:
    std::size_t new_module_index_()
    {
        static std::size_t index = 0;
        return index++;
    }

private:
    std::string name_;
    std::stop_token stop_token_;
};

}
}
