//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_SPECIALIZATION_OF_HPP
#define XSTD_CONCEPTS_SPECIALIZATION_OF_HPP

#include <xstd/type_traits/is_specialization_of.hpp> // is_specialization_of_v

namespace xstd {

// The constraint spelling of xstd::is_specialization_of.
template<class T, template<class...> class Primary>
concept specialization_of = is_specialization_of_v<T, Primary>;

} // namespace xstd

#endif // XSTD_CONCEPTS_SPECIALIZATION_OF_HPP
