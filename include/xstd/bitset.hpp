#pragma once

//          Copyright Rein Halbersma 2014-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset>       // bitset
#include <utility>      // move

namespace xstd {

template<auto N, class UnaryFunction>
auto for_each(std::bitset<N> const& bs, UnaryFunction fun)
{
        for (auto i = bs._Find_first(); i < N; i = bs._Find_next(i)) {
                fun(i);
        }
        return std::move(fun);
}

template<auto N>
auto operator<(std::bitset<N> const& lhs, std::bitset<N> const& rhs)
{
        return lhs.to_string() < rhs.to_string();
}

template<auto N>
auto operator>(std::bitset<N> const& lhs, std::bitset<N> const& rhs)
{
        return rhs < lhs;
}

template<auto N>
auto operator>=(std::bitset<N> const& lhs, std::bitset<N> const& rhs)
{
        return !(lhs < rhs);
}

template<auto N>
auto operator<=(std::bitset<N> const& lhs, std::bitset<N> const& rhs)
{
        return !(rhs < lhs);
}

template<auto N>
auto is_subset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return (lhs & ~rhs).none();
}

template<auto N>
auto is_superset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return is_subset_of(rhs, lhs);
}

template<auto N>
auto is_proper_subset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return is_subset_of(lhs, rhs) && !is_subset_of(rhs, lhs);
}

template<auto N>
auto is_proper_superset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return is_superset_of(lhs, rhs) && !is_superset_of(rhs, lhs);
}

template<auto N>
auto intersects(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return (lhs & rhs).any();
}

template<auto N>
auto disjoint(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return !intersects(lhs, rhs);
}

template<auto N>
auto operator-(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return lhs & ~rhs;
}

}       // namespace xstd
