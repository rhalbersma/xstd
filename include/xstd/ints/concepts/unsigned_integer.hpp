//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_UNSIGNED_INTEGER_HPP
#define XSTD_CONCEPTS_UNSIGNED_INTEGER_HPP

#include <xstd/ints/concepts/integer.hpp>        // integer
#include <xstd/ints/type_traits/is_unsigned.hpp> // is_unsigned_v

namespace xstd {

// Mirrors std::unsigned_integral, keeping integer atomic for subsumption.
template<class T>
concept unsigned_integer = integer<T> and is_unsigned_v<T>;

} // namespace xstd

#endif // XSTD_CONCEPTS_UNSIGNED_INTEGER_HPP
