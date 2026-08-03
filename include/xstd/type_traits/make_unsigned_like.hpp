//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_MAKE_UNSIGNED_LIKE_HPP
#define XSTD_TYPE_TRAITS_MAKE_UNSIGNED_LIKE_HPP

#include <xstd/cstdint.hpp>
#include <xstd/type_traits/is_integral_like.hpp>
#include <xstd/type_traits/is_unsigned_like.hpp>
#include <type_traits>

namespace xstd {

template<class T>
struct make_unsigned_like
{};

template<class T>
        requires std::is_integral_v<T> and (not std::is_same_v<std::remove_cv_t<T>, bool>)
struct make_unsigned_like<T> : std::make_unsigned<T>
{};

template<class T>
        requires (not std::is_integral_v<T>) and is_integral_like_v<T> and is_unsigned_like_v<T>
struct make_unsigned_like<T> : std::type_identity<T>
{};

template<class T>
using make_unsigned_like_t = make_unsigned_like<T>::type;

template<>
struct make_unsigned_like<int128_t> : std::type_identity<uint128_t>
{};

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_MAKE_UNSIGNED_LIKE_HPP
