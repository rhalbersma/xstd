//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_IS_CHARACTER_HPP
#define XSTD_TYPE_TRAITS_IS_CHARACTER_HPP

#include <type_traits> // bool_constant

namespace xstd {

template<class T>
inline constexpr auto is_character_v = false;

template<>
inline constexpr auto is_character_v<char> = true;

template<>
inline constexpr auto is_character_v<wchar_t> = true;

template<>
inline constexpr auto is_character_v<char8_t> = true;

template<>
inline constexpr auto is_character_v<char16_t> = true;

template<>
inline constexpr auto is_character_v<char32_t> = true;

template<class T>
using is_character = std::bool_constant<is_character_v<T>>;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_IS_CHARACTER_HPP
