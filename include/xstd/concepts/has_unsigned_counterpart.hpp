//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_HAS_UNSIGNED_COUNTERPART_HPP
#define XSTD_CONCEPTS_HAS_UNSIGNED_COUNTERPART_HPP

#include <xstd/type_traits/make_unsigned.hpp> // make_unsigned_t

namespace xstd {

// Whether make_unsigned names a type for T, which unsigned_abs and to_chars produce.
template<class T>
concept has_unsigned_counterpart = requires { typename make_unsigned_t<T>; };

} // namespace xstd

#endif // XSTD_CONCEPTS_HAS_UNSIGNED_COUNTERPART_HPP
