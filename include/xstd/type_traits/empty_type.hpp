//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_EMPTY_TYPE_HPP
#define XSTD_TYPE_TRAITS_EMPTY_TYPE_HPP

#include <compare>     // strong_ordering
#include <concepts>    // same_as
#include <type_traits> // remove_cvref_t

namespace xstd {

template<class Tag>
struct empty_type
{
        // Both specifiers are implicit for a defaulted function; [[nodiscard]] is not.
        [[nodiscard]] empty_type() = default;

        // Constructible like any alternative member, without hijacking copy or move.
        template<class... Args>
                requires ((not std::same_as<std::remove_cvref_t<Args>, empty_type>) and ...)
        [[nodiscard]] constexpr explicit empty_type(Args&&...) noexcept
        {}

        // Lets an enclosing class default its comparisons over this member.
        [[nodiscard]] friend auto operator<=>(empty_type, empty_type) -> std::strong_ordering = default;
};

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_EMPTY_TYPE_HPP
