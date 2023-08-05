#pragma once

#include <arba/appt/application/module/module_logger.hpp>
#include <arba/appt/application/program_args.hpp>
#include <spdlog/spdlog.h>

inline namespace arba
{
namespace appt
{
inline namespace mdec // module_decorator
{

template <class module_logger_type, class module_base_type, class module_type = void>
class logging;

template <class module_logger_type, class module_base_type>
class logging<module_logger_type, module_base_type> : public module_base_type
{
private:
    using base_ = module_base_type;

public:
    using application_type = typename module_base_type::application_type;

    template <typename other_module_type>
    using rebind_t = logging<module_logger_type, module_base_type, other_module_type>;

    explicit logging(std::string_view name = std::string_view()) : base_(name) {}
    virtual ~logging() override = default;

    inline const std::shared_ptr<spdlog::logger>& logger() const { return logger_; }
    inline std::shared_ptr<spdlog::logger>& logger() { return logger_; }
    void set_app(application_type& app)
    {
        if (logger_)
        {
            spdlog::drop(logger_->name());
            logger_.reset();
        }
        this->base_::set_app(app);
        logger_ = std::make_shared<module_logger_type>(*this);
        spdlog::register_logger(logger_);
    }

private:
    std::shared_ptr<spdlog::logger> logger_;
};

template <class module_logger_type, class module_base_type, class module_type>
class logging : public logging<module_logger_type, typename module_base_type::template rebind_t<module_type>>
{
private:
    using base_ = logging<module_logger_type, typename module_base_type::template rebind_t<module_type>>;

public:
    using base_::base_;
    virtual ~logging() override = default;
};

}
}
}
