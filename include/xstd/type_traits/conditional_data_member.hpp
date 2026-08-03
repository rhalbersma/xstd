//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_CONDITIONAL_DATA_MEMBER_HPP
#define XSTD_TYPE_TRAITS_CONDITIONAL_DATA_MEMBER_HPP

#include <compare>
#include <type_traits>

namespace xstd {

template<class Tag>
struct empty_type
{
        [[nodiscard]] constexpr empty_type() noexcept = default;

        template<class... Args>
                requires ((not std::is_same_v<std::remove_cvref_t<Args>, empty_type>) and ...)
        [[nodiscard]] constexpr explicit empty_type(Args&&...) noexcept
        {}

        [[nodiscard]] friend constexpr auto operator<=>(empty_type, empty_type) noexcept -> std::strong_ordering = default;
};

template<bool Condition, class Type, class Tag>
using conditional_data_member_t = std::conditional_t<Condition, Type, empty_type<Tag>>;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_CONDITIONAL_DATA_MEMBER_HPP
