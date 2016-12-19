#pragma once
#include <type_traits>  // enable_if, is_enum, underlying_type

namespace xstd {

template<class Enumeration, std::enable_if_t<std::is_enum<Enumeration>{}>...>
constexpr auto to_underlying_type(Enumeration e) noexcept
{
        return static_cast<std::underlying_type_t<Enumeration>>(e);
}

}       // namespace xstd
