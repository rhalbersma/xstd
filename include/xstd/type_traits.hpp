#pragma once
#include <type_traits>  // is_unsigned, is_integral, underlying_type

namespace xstd {

template<class T>
struct is_unsigned : std::is_unsigned<T> {};

template<>
struct is_unsigned<__uint128_t> : std::true_type {};

template<>
struct is_unsigned<__int128_t> : std::false_type {};

template<class T>
struct is_integral : std::is_integral<T> {};

template<>
struct is_integral<__uint128_t> : std::true_type {};

template<>
struct is_integral<__int128_t> : std::true_type {};

template<class E>
constexpr auto to_underlying_type(E const e) noexcept
{
        static_assert(std::is_enum<E>{});
        return static_cast<std::underlying_type_t<E>>(e);
}

template<class T>
constexpr auto is_trivial_special_members_v =
        std::is_trivially_destructible<T>{} &&
        std::is_trivially_default_constructible<T>{} &&
        std::is_trivially_copy_constructible<T>{} &&
        std::is_trivially_copy_assignable<T>{} &&
        std::is_trivially_move_constructible<T>{} &&
        std::is_trivially_move_assignable<T>{}
;

}       // namespace xstd
