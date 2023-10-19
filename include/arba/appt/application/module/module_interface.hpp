#pragma once

#include <source_location>
#include <string>
#include <exception>
#include <arba/core/sbrm.hpp>
#include <arba/appt/application/execution_status.hpp>

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
    virtual void stop() {}

    void run_maythrow();

    template <typename ApplicationType>
    void run_nothrow(ApplicationType& app)
    {
        try
        {
            run_maythrow();
        }
        catch (...)
        {
            this->handle_caught_exception_(std::source_location::current(), std::current_exception());
            if constexpr (requires(ApplicationType& app){ { app.stop() }; })
            {
                app.stop();
            }
        }
    }

    [[nodiscard]] inline execution_status run_status() const noexcept { return run_status_; }

protected:
    virtual void run() = 0;
    virtual void handle_caught_exception_(const std::source_location& location, std::exception_ptr ex_ptr) = 0;

private:
    std::size_t new_module_index_()
    {
        static std::size_t index = 0;
        return index++;
    }

private:
    std::string name_;
    execution_status run_status_ = execution_status::ready;
};

}
}
