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
        // [[nodiscard]] and nothing else: a function explicitly defaulted on
        // its first declaration is implicitly constexpr if the implicit
        // declaration would be, and its exception specification is computed
        // from what it does. Over no members at all both come out as they
        // would have been written, so writing them restates a computation.
        // [[nodiscard]] has no implicit form, which is why it is here.
        [[nodiscard]] empty_type() = default;

        // Lets an enclosing class construct this like any alternative member,
        // without allowing the catch-all to hijack copy or move construction.
        // This one has a body rather than being defaulted, and a body is not
        // deduced from, so it says both.
        template<class... Args>
                requires ((not std::is_same_v<std::remove_cvref_t<Args>, empty_type>) and ...)
        [[nodiscard]] constexpr explicit empty_type(Args&&...) noexcept
        {}

        // Lets an enclosing class default comparisons over this member, and is
        // defaulted itself, so it is spelled the same way as the constructor
        // above it. The == that std::regular wants comes with it: a defaulted
        // three-way comparison implicitly declares one.
        [[nodiscard]] friend auto operator<=>(empty_type, empty_type) -> std::strong_ordering = default;
};

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_EMPTY_TYPE_HPP
