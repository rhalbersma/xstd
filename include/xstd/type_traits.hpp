#pragma once
#include <type_traits>  // enable_if, is_enum, underlying_type, integral_constant

namespace xstd {

template<class Enumeration, std::enable_if_t<
        std::is_enum_v<Enumeration>
>...>
constexpr auto to_underlying_type(Enumeration e) noexcept
{
        return static_cast<std::underlying_type_t<Enumeration>>(e);
}

template<class Enumeration, Enumeration N, std::enable_if_t<
        std::is_enum_v<Enumeration>
>...>
constexpr auto to_underlying_type(std::integral_constant<Enumeration, N>) noexcept
{
        return static_cast<std::underlying_type_t<Enumeration>>(N);
}

}       // namespace xstd
