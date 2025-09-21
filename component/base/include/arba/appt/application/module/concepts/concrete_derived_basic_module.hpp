#pragma once

#include <arba/appt/application/module/basic_module.hpp>

#include <type_traits>

inline namespace arba
{
namespace appt
{

inline namespace concepts
{
template <typename ModType>
concept ConcreteDerivedBasicModule =
    (!std::is_abstract_v<ModType>)
    && std::is_base_of_v<basic_module<typename ModType::application_type, typename ModType::self_type>, ModType>;
}

} // namespace appt
} // namespace arba
