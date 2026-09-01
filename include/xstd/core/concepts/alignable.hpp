//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CORE_CONCEPTS_ALIGNABLE_HPP
#define XSTD_CORE_CONCEPTS_ALIGNABLE_HPP

#include <concepts> // totally_ordered
#include <cstddef>  // size_t

namespace xstd {

// The operations align_up and align_down perform, and the wraparound they rely on.
// Boost.Align asks only that the type not be a pointer; naming the operations says
// the same about pointers, since ~p does not compile, and more about everything else.
template<class I>
concept alignable =
        std::totally_ordered<I> and
        requires (I x, std::size_t n) {
                { static_cast<I>(n) } -> std::same_as<I>;
                { static_cast<I>(x + x) } -> std::same_as<I>;
                { static_cast<I>(x - x) } -> std::same_as<I>;
                { static_cast<I>(x & x) } -> std::same_as<I>;
                { static_cast<I>(~x) } -> std::same_as<I>;
        } and
        // Modular: zero decremented is the largest value, so nothing lies below zero to
        // align into. Said in I, because the narrow types promote out of themselves first.
        static_cast<I>(I{} - I{1}) > I{} and
        // And it counts, rather than saturating: bool's one plus one is still one.
        static_cast<I>(I{1} + I{1}) > I{1};

// Whether those same operations carry noexcept, which a class type need not: Abseil's
// 128-bit types declare it nowhere, and are alignable all the same.
template<class I>
concept nothrow_alignable =
        alignable<I> and
        requires (I x, std::size_t n) {
                { static_cast<I>(n) } noexcept;
                { static_cast<I>(x + x) } noexcept;
                { static_cast<I>(x - x) } noexcept;
                { static_cast<I>(x & x) } noexcept;
                { static_cast<I>(~x) } noexcept;
                { x >= x } noexcept;
        };

} // namespace xstd

#endif // XSTD_CORE_CONCEPTS_ALIGNABLE_HPP
