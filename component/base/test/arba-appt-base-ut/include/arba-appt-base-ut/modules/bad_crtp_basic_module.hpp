#pragma once

#include <arba/appt/application/module/basic_module.hpp>

namespace ut
{

template <class ApplicationType>
class bad_crtp_module : public appt::basic_module<ApplicationType>
{
private:
    using base_ = appt::basic_module<ApplicationType>;

public:
    using base_::base_;

    virtual ~bad_crtp_module() override = default;

    virtual void init() override
    {
        this->base_::init();
        std::abort();
    }

    virtual void run() override { std::abort(); }
};

} // namespace ut
