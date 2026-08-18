//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_HPP
#define XSTD_TYPE_TRAITS_HPP

// Helper classes
#include <xstd/type_traits/empty_type.hpp> // IWYU pragma: export; empty_type

// Type properties
#include <xstd/type_traits/is_character.hpp> // IWYU pragma: export; is_character_v
#include <xstd/type_traits/is_signed.hpp>   // IWYU pragma: export; is_signed
#include <xstd/type_traits/is_unsigned.hpp> // IWYU pragma: export; is_unsigned

// Type property specifiers
#include <xstd/type_traits/no_unique_address.hpp> // IWYU pragma: export; XSTD_NO_UNIQUE_ADDRESS

// Type relationships
#include <xstd/type_traits/is_specialization_of.hpp> // IWYU pragma: export; is_specialization_of

// Sign modifiers
#include <xstd/type_traits/make_signed.hpp>   // IWYU pragma: export; make_signed
#include <xstd/type_traits/make_unsigned.hpp> // IWYU pragma: export; make_unsigned

// Miscellaneous transformations
#include <xstd/type_traits/conditional_data_member.hpp> // IWYU pragma: export; conditional_data_member_t

#endif // XSTD_TYPE_TRAITS_HPP
