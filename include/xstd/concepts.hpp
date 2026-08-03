//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_HPP
#define XSTD_CONCEPTS_HPP

#include <xstd/type_traits.hpp> // is_integral_like_v, is_signed_like_v, is_specialization_of_v

namespace xstd {

// The constraint spelling of xstd::is_specialization_of.
template<class T, template<class...> class Primary>
concept specialization_of = is_specialization_of_v<T, Primary>;

// The open counterpart of std::integral, admitting integer-class types and
// implementation-provided extended integers in addition to built-in integers.
template<class T>
concept integral_like = is_integral_like_v<T>;

// These mirror std::signed_integral and std::unsigned_integral while preserving
// integral_like as an atomic constraint for subsumption.
template<class T>
concept signed_integral_like = integral_like<T> and is_signed_like_v<T>;

template<class T>
concept unsigned_integral_like = integral_like<T> and (not is_signed_like_v<T>);

} // namespace xstd

#endif // XSTD_CONCEPTS_HPP
