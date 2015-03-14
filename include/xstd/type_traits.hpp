#pragma once
#include <type_traits>

namespace xstd {

// www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3911.pdf
template<class...>
using void_t = void;

template<class T>
struct type_is
{
        using type = T;
};

// http://www.reddit.com/r/cpp/comments/2pey6r/tutorial_on_tag_dispatching_crazy_eddies_crazy_c/cmwal2r
template<class T>
struct always_false
:
        std::false_type
{};

template<class T>
using always_false_t = typename always_false<T>::type;

template<class T>
constexpr auto always_false_v = always_false<T>::value;

template<class E>
constexpr auto to_underlying_type(E e) noexcept
{
        return static_cast<std::underlying_type_t<E>>(e);
}

}       // namespace xstd
