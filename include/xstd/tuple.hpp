#pragma once
#include <cstddef>      // size-t
#include <tuple>        // get, tuple

namespace xstd {

template <class T, class U>
constexpr auto compare(T const& lhs, U const& rhs) noexcept
{
        return (lhs < rhs) ? -1 : (rhs < lhs) ? +1 : 0;
}
namespace detail {

template<size_t N>
struct compare
{
        template<class T, class U>
        constexpr auto operator()(T const& lhs, U const& rhs) const noexcept
        {
                auto const r = compare<N - 1>{}(lhs, rhs);
                return (r != 0) ? r : xstd::compare(std::get<N - 1>(lhs), std::get<N - 1>(rhs));
        }
};

template<>
struct compare<0>
{
        template<class T, class U>
        constexpr auto operator()(T const& /* lhs */, U const& /* rhs */) const noexcept
        {
                return 0;
        }
};

}       // namespace detail

template<class... Ts, class... Us>
constexpr auto compare(std::tuple<Ts...> const& lhs, std::tuple<Us...> const& rhs) noexcept
{
        static_assert(sizeof...(Ts) == sizeof...(Us));
        return detail::compare<sizeof...(Ts)>{}(lhs, rhs);
}

}       // namespace xstd
