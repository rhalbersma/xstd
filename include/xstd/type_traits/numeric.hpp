//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_NUMERIC_HPP
#define XSTD_TYPE_TRAITS_NUMERIC_HPP

#include <xstd/concepts/exposition_only/integral-class.hpp> // is_integral_like
#include <type_traits>                                      // bool_constant, is_floating_point_v

namespace xstd {

// std::is_integral opened to structurally conforming integer-class types.
template<class T>
inline constexpr auto is_integral_like_v = exposition_only::is_integral_like<T>;

template<class T>
using is_integral_like = std::bool_constant<is_integral_like_v<T>>;

// std::is_arithmetic with its integral half opened to integral-like types.
template<class T>
inline constexpr auto is_arithmetic_like_v = is_integral_like_v<T> or std::is_floating_point_v<T>;

template<class T>
using is_arithmetic_like = std::bool_constant<is_arithmetic_like_v<T>>;

// std::is_signed and std::is_unsigned with the arithmetic test opened.
template<class T>
inline constexpr auto is_signed_like_v = false;

template<class T>
        requires is_arithmetic_like_v<T>
// Both checks fire only on the bool instantiation, where -1 and 0 are the
// standard's own spelling of this test and turning them into bool literals
// would change what is being asked. T is a template parameter, so neither
// diagnostic is about the code as written.
// NOLINTNEXTLINE(readability-implicit-bool-conversion,modernize-use-bool-literals)
inline constexpr auto is_signed_like_v<T> = static_cast<T>(-1) < static_cast<T>(0);

template<class T>
using is_signed_like = std::bool_constant<is_signed_like_v<T>>;

template<class T>
inline constexpr auto is_unsigned_like_v = false;

template<class T>
        requires is_arithmetic_like_v<T>
// NOLINTNEXTLINE(readability-implicit-bool-conversion,modernize-use-bool-literals)
inline constexpr auto is_unsigned_like_v<T> = static_cast<T>(0) < static_cast<T>(-1);

template<class T>
using is_unsigned_like = std::bool_constant<is_unsigned_like_v<T>>;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_NUMERIC_HPP
