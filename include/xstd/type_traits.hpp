#pragma once
#include <type_traits>  // underlying_type, void_t (C++1z)

namespace xstd {

// www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3911.pdf

template<class T>
struct type_is
{
        using type = T;
};

// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4436.pdf

// primary template handles all types not supporting the archetypal Op
template
<
        class Default,
        class, // always void; supplied externally
        template<class...> class Op,
        class... Args
>
struct detector
{
        constexpr static auto value = false;
        using type = Default;
};

// specialization recognizes and handles only types supporting Op
template
<
        class Default,
        template<class...> class Op,
        class... Args
>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...>
{
        constexpr static auto value = true;
        using type = Op<Args...>;
};

template<template<class...> class Op, class... Args>
using is_detected = detector<void, void, Op, Args...>;

template<template<class...> class Op, class... Args>
constexpr auto is_detected_v = is_detected<Op, Args...>::value;

template<template<class...> class Op, class... Args>
using detected_t = typename is_detected<Op, Args...>::type;

template<class Default, template<class...> class Op, class... Args>
using detected_or = detector<Default, void, Op, Args...>;

template<class Default, template<class...> class Op, class... Args>
using detected_or_t = typename detected_or<Default, Op, Args...>::type;

template<class Expected, template<class...> class Op, class... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

template<class Expected, template<class...> class Op, class... Args >
constexpr auto is_detected_exact_v = is_detected_exact<Expected, Op, Args...>::value;

template<class To, template<class...> class Op, class... Args>
using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

template<class To, template<class...> class Op, class... Args>
constexpr auto is_detected_convertible_v = is_detected_convertible<To, Op, Args...>::value;

struct nonesuch
{
        ~nonesuch() = delete;
        nonesuch() = delete;
        nonesuch(nonesuch const&) = delete;
        nonesuch& operator=(nonesuch const&) = delete;
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

namespace block_adl {

template<class>
struct empty_base
{
        empty_base() = default;

        template<class... Args>
        constexpr empty_base(Args&&...) {}
};

template<bool Condition, class T>
using optional_base = std::conditional_t<Condition, T, empty_base<T>>;

}       // namespace block_adl

// deriving from empty_base or optional_base will not make xstd an associated namespace
// this prevents ADL from finding overloads for e.g. begin/end/swap from namespace xstd
using block_adl::empty_base;
using block_adl::optional_base;

}       // namespace xstd
