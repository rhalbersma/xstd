//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_HPP
#define XSTD_TYPE_TRAITS_HPP

// Helper classes
#include <xstd/type_traits/empty_type.hpp> // IWYU pragma: export; empty_type

// Primary type categories
#include <xstd/type_traits/is_integral_like.hpp> // IWYU pragma: export; is_integral_like

// Composite type categories
#include <xstd/type_traits/is_arithmetic_like.hpp> // IWYU pragma: export; is_arithmetic_like

// Type properties
#include <xstd/type_traits/is_signed_like.hpp>   // IWYU pragma: export; is_signed_like
#include <xstd/type_traits/is_unsigned_like.hpp> // IWYU pragma: export; is_unsigned_like

// Type relationships
#include <xstd/type_traits/is_specialization_of.hpp> // IWYU pragma: export; is_specialization_of

// Sign modifiers
#include <xstd/type_traits/make_signed_like.hpp>   // IWYU pragma: export; make_signed_like
#include <xstd/type_traits/make_unsigned_like.hpp> // IWYU pragma: export; make_unsigned_like

// Miscellaneous transformations
#include <xstd/type_traits/conditional_data_member.hpp> // IWYU pragma: export; XSTD_NO_UNIQUE_ADDRESS, conditional_data_member_t

#endif // XSTD_TYPE_TRAITS_HPP
