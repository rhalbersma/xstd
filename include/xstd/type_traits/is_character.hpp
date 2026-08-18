//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_IS_CHARACTER_HPP
#define XSTD_TYPE_TRAITS_IS_CHARACTER_HPP

#include <type_traits> // bool_constant

namespace xstd {

template<class T>
struct is_character : std::bool_constant<false>
{};

template<>
struct is_character<char> : std::bool_constant<true>
{};
template<>
struct is_character<wchar_t> : std::bool_constant<true>
{};
template<>
struct is_character<char8_t> : std::bool_constant<true>
{};
template<>
struct is_character<char16_t> : std::bool_constant<true>
{};
template<>
struct is_character<char32_t> : std::bool_constant<true>
{};

template<class T>
inline constexpr auto is_character_v = is_character<T>::value;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_IS_CHARACTER_HPP
