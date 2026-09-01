//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CONCEPTS_NOTHROW_REGULAR_HPP
#define XSTD_INTS_CONCEPTS_NOTHROW_REGULAR_HPP

#include <concepts>    // regular, same_as
#include <type_traits> // is_nothrow_..._v, remove_cv_t

namespace xstd {

// std::regular's own requirements, asked again for noexcept. integer_class asks /9
// for regularity and the nothrow concepts then asked only after the operators, so a
// type could carry noexcept on every one of them and still throw on the way in, out
// or across: default construction, assignment and swap were nobody's question.
//
// Not itself about integers, but xstd/core and xstd/ints are leaves that may not
// include one another, and both users of this are here.
template<class T_cv, class T = std::remove_cv_t<T_cv>>
concept nothrow_regular =
        // T is the parameter's own default; naming it explicitly cannot redirect the question.
        std::same_as<T, std::remove_cv_t<T_cv>> and
        std::regular<T> and

        // The way out, which every by-value return travels.
        std::is_nothrow_destructible_v<T> and

        // The ways in: /9's default initialization, then the two copies.
        std::is_nothrow_default_constructible_v<T> and
        std::is_nothrow_move_constructible_v<T> and
        std::is_nothrow_copy_constructible_v<T> and

        // And across, which no cast expression reaches and so nothing else asked.
        std::is_nothrow_move_assignable_v<T> and
        std::is_nothrow_copy_assignable_v<T> and
        std::is_nothrow_swappable_v<T> and

        // regular's equality half, stated as the operators rather than as the traits.
        requires (T const a, T const b) {
                { a == b } noexcept;
                { a != b } noexcept;
        };

} // namespace xstd

#endif // XSTD_INTS_CONCEPTS_NOTHROW_REGULAR_HPP
