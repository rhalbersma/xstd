//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_HPP
#define XSTD_CONCEPTS_HPP

// Core language concepts
#include <xstd/concepts/integer_class.hpp>            // IWYU pragma: export; integer_class
#include <xstd/concepts/integer_class_operations.hpp> // IWYU pragma: export; integer_class_operations
#include <xstd/concepts/integer_like.hpp>             // IWYU pragma: export; integer_like
#include <xstd/concepts/signed_integer_like.hpp>      // IWYU pragma: export; signed_integer_like
#include <xstd/concepts/unsigned_integer_like.hpp>    // IWYU pragma: export; unsigned_integer_like
#include <xstd/concepts/specialization_of.hpp>        // IWYU pragma: export; specialization_of

// Domain and exception specification of the integer functions
#include <xstd/concepts/nothrow_integer_operators.hpp> // IWYU pragma: export; nothrow_integer_operators

#endif // XSTD_CONCEPTS_HPP
