//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_HPP
#define XSTD_CONCEPTS_HPP

// Core language concepts
#include <xstd/concepts/integral_like.hpp>          // IWYU pragma: export; integral_like
#include <xstd/concepts/signed_integral_like.hpp>   // IWYU pragma: export; signed_integral_like
#include <xstd/concepts/unsigned_integral_like.hpp> // IWYU pragma: export; unsigned_integral_like
#include <xstd/concepts/specialization_of.hpp>      // IWYU pragma: export; specialization_of

// Domain and exception specification of the integer functions
#include <xstd/concepts/has_unsigned_counterpart.hpp>   // IWYU pragma: export; has_unsigned_counterpart
#include <xstd/concepts/nothrow_integral_operators.hpp> // IWYU pragma: export; nothrow_integral_operators

#endif // XSTD_CONCEPTS_HPP
