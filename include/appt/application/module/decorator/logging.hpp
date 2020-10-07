#pragma once

#include <appt/application/module/module_logger.hpp>
#include <appt/application/program_args.hpp>
#include <spdlog/spdlog.h>

inline namespace arba
{
namespace appt::mdec
{

template <class module_logger_type, class module_base_type>
class logging : public module_base_type
{
private:
    using base_ = module_base_type;
    using application_type = typename base_::application_type;

public:
    logging() {}
    explicit logging(std::string name) : base_(std::move(name)) {}
    explicit logging(application_type& app) : base_(app), logger_(std::make_shared<module_logger_type>(*this)) {}
    logging(std::string name, application_type& app) : base_(std::move(name), app), logger_(std::make_shared<module_logger>(*this)) {}
    virtual ~logging() override = default;

    inline const std::shared_ptr<spdlog::logger>& logger() const { return logger_; }
    inline std::shared_ptr<spdlog::logger>& logger() { return logger_; }
    void set_app(application_type& app)
    {
        this->base_::set_app(app);
        logger_ = std::make_shared<module_logger_type>(*this);
    }

private:
    std::shared_ptr<spdlog::logger> logger_;
};

}
}
