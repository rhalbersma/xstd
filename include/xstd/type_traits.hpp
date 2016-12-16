#pragma once
#include <type_traits>  // is_unsigned, is_integral, false_type, true_type, is_enum, underlying_type

namespace xstd {

template<class T> struct is_unsigned : std::is_unsigned<T> {};
template<>        struct is_unsigned<__int128_t> : std::false_type {};
template<>        struct is_unsigned<__uint128_t> : std::true_type {};

template<class T> struct is_integral : std::is_integral<T> {};
template<>        struct is_integral<__int128_t> : std::true_type {};
template<>        struct is_integral<__uint128_t> : std::true_type {};

template<class E>
constexpr auto to_underlying_type(E const e) noexcept
{
        static_assert(std::is_enum<E>{});
        return static_cast<std::underlying_type_t<E>>(e);
}

}       // namespace xstd
