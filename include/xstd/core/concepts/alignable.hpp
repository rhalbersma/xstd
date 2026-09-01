//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CORE_CONCEPTS_ALIGNABLE_HPP
#define XSTD_CORE_CONCEPTS_ALIGNABLE_HPP

#include <concepts>    // constructible_from, totally_ordered
#include <cstddef>     // size_t
#include <type_traits> // is_nothrow_constructible_v

namespace xstd {

// T is addressable: it holds a size_t and does the modular arithmetic align_up and align_down perform.
template<class T>
concept alignable =
        std::constructible_from<T, std::size_t> and
        std::totally_ordered<T> and
        requires (T x) {
                static_cast<T>(x + x);
                static_cast<T>(x - x);
                static_cast<T>(x & x);
                static_cast<T>(~x);
        } and
        // Modular: zero decremented is the largest value, said in T because the narrow types promote out of it.
        static_cast<T>(T{} - T{1}) > T{} and
        // And it counts, rather than saturating: bool's one plus one is still one.
        static_cast<T>(T{1} + T{1}) > T{1};

// Whether those operations carry noexcept, which Abseil's 128-bit types do not, being alignable all the same.
template<class T>
concept nothrow_alignable =
        alignable<T> and
        std::is_nothrow_constructible_v<T, std::size_t> and
        requires (T x) {
                { static_cast<T>(x + x) } noexcept;
                { static_cast<T>(x - x) } noexcept;
                { static_cast<T>(x & x) } noexcept;
                { static_cast<T>(~x) } noexcept;
                { x >= x } noexcept;
        };

} // namespace xstd

#endif // XSTD_CORE_CONCEPTS_ALIGNABLE_HPP
