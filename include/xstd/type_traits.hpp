#pragma once
#include <type_traits>  // underlying_type

namespace xstd {

template<class T>
struct type_is
{
        using type = T;
};

template<class E>
constexpr auto to_underlying_type(E e) noexcept
{
        return static_cast<std::underlying_type_t<E>>(e);
}

template<class T>
using value_t = typename T::value_type;

template<class T>
using _t = typename T::type;

}       // namespace xstd
