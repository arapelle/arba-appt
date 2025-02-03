#pragma once

#include <arba/appt/application/module/module.hpp>

namespace ut
{

template <class ApplicationType>
class counting_module : public appt::module<ApplicationType, counting_module<ApplicationType>>
{
private:
    using base_ = appt::module<ApplicationType, counting_module<ApplicationType>>;

public:
    using base_::base_;

    virtual ~counting_module() override = default;

    virtual void init() override
    {
        this->base_::init();
        ++init_count;
    }

    virtual void run() override { ++run_count; }

    uint16_t run_count = 0;
    uint16_t init_count = 0;
};

} // namespace ut
