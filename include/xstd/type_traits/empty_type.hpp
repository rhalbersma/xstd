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
        // constexpr and the exception specification are both implicit for a
        // defaulted function; [[nodiscard]] is not. See doc/design.md.
        [[nodiscard]] empty_type() = default;

        // Lets an enclosing class construct this like any alternative member,
        // without allowing the catch-all to hijack copy or move construction.
        // Written rather than defaulted, so it spells both specifiers itself.
        template<class... Args>
                requires ((not std::is_same_v<std::remove_cvref_t<Args>, empty_type>) and ...)
        [[nodiscard]] constexpr explicit empty_type(Args&&...) noexcept
        {}

        // Lets an enclosing class default comparisons over this member; the ==
        // that std::regular wants comes with a defaulted three-way comparison.
        [[nodiscard]] friend auto operator<=>(empty_type, empty_type) -> std::strong_ordering = default;
};

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_EMPTY_TYPE_HPP
