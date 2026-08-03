//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_EMPTY_TYPE_HPP
#define XSTD_TYPE_TRAITS_EMPTY_TYPE_HPP

#include <compare>     // strong_ordering
#include <type_traits> // is_same_v, remove_cvref_t

namespace xstd {

template<class Tag>
struct empty_type
{
        [[nodiscard]] constexpr empty_type() noexcept = default;

        // Lets an enclosing class construct this like any alternative member,
        // without allowing the catch-all to hijack copy or move construction.
        template<class... Args>
                requires ((not std::is_same_v<std::remove_cvref_t<Args>, empty_type>) and ...)
        [[nodiscard]] constexpr explicit empty_type(Args&&...) noexcept
        {}

        // Lets an enclosing class default comparisons over this member.
        [[nodiscard]] friend constexpr auto operator<=>(empty_type, empty_type) noexcept -> std::strong_ordering = default;
};

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_EMPTY_TYPE_HPP
