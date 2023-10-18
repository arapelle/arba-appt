#pragma once

#include <arba/appt/application/module/module.hpp>

namespace ut
{

template <class ApplicationType>
class counting_module : public appt::module<ApplicationType>
{
public:
    counting_module(std::string_view module_name = std::string_view())
        : appt::module<ApplicationType>(module_name)
    {}

    virtual ~counting_module() override = default;

    virtual void init() override
    {
        ++init_count;
    }

    virtual void run() override
    {
        ++run_count;
    }

    uint16_t run_count = 0;
    uint16_t init_count = 0;
};

}
