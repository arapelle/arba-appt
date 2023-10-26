#pragma once

#include <arba/appt/application/module/module.hpp>

namespace ut
{

template <class ApplicationType>
class bad_crtp_module : public appt::module<ApplicationType>
{
private:
    using base_ = appt::module<ApplicationType>;

public:
    using base_::base_;

    virtual ~bad_crtp_module() override = default;

    virtual void init() override
    {
        this->base_::init();
        std::abort();
    }

    virtual void run() override
    {
        std::abort();
    }
};

}
