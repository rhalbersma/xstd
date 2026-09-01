//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CORE_TYPE_TRAITS_EMPTY_TYPE_HPP
#define XSTD_CORE_TYPE_TRAITS_EMPTY_TYPE_HPP

#include <compare>     // strong_ordering
#include <concepts>    // same_as
#include <type_traits> // remove_cvref_t

namespace xstd {

// The tag keeps two empty data members distinct in a layout.
template<class Tag = void>
struct empty_type
{
        [[nodiscard]] empty_type() = default;

        // Lets an enclosing class construct this member, without hijacking copy or move construction.
        template<class... Args>
                requires ((not std::same_as<std::remove_cvref_t<Args>, empty_type>) and ...)
        [[nodiscard]] constexpr explicit empty_type(Args&&...) noexcept
        {}

        // Lets an enclosing class default its comparisons over this member.
        [[nodiscard]] friend auto operator<=>(empty_type, empty_type) -> std::strong_ordering = default;
};

} // namespace xstd

#endif // XSTD_CORE_TYPE_TRAITS_EMPTY_TYPE_HPP
