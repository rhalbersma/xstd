#pragma once
#include <type_traits>  // underlying_type, void_t (C++1z)

namespace xstd {

template<class T>
struct type_is
{
        using type = T;
};

  /// Implementation of the detection idiom (negative case).
  template<typename _Default, typename _AlwaysVoid,
           template<typename...> class _Op, typename... _Args>
    struct __detector
    {
      using value_t = std::false_type;
      using type = _Default;
    };

  /// Implementation of the detection idiom (positive case).
  template<typename _Default, template<typename...> class _Op,
            typename... _Args>
    struct __detector<_Default, std::void_t<_Op<_Args...>>, _Op, _Args...>
    {
      using value_t = std::true_type;
      using type = _Op<_Args...>;
    };

  // Detect whether _Op<_Args...> is a valid type, use _Default if not.
  template<typename _Default, template<typename...> class _Op,
           typename... _Args>
    using __detected_or = __detector<_Default, void, _Op, _Args...>;

  // _Op<_Args...> if that is a valid type, otherwise _Default.
  template<typename _Default, template<typename...> class _Op,
           typename... _Args>
    using __detected_or_t
      = typename __detected_or<_Default, _Op, _Args...>::type;

  // _Op<_Args...> if that is a valid type, otherwise _Default<_Args...>.
  template<template<typename...> class _Default,
           template<typename...> class _Op, typename... _Args>
    using __detected_or_t_ =
      __detected_or_t<_Default<_Args...>, _Op, _Args...>;

struct nonesuch
{
  nonesuch() = delete;
  ~nonesuch() = delete;
  nonesuch(nonesuch const&) = delete;
  void operator=(nonesuch const&) = delete;
};

template<template<typename...> class _Op, typename... _Args>
  using is_detected
    = typename __detector<nonesuch, void, _Op, _Args...>::value_t;

template<template<typename...> class _Op, typename... _Args>
  constexpr bool is_detected_v = is_detected<_Op, _Args...>::value;

template<template<typename...> class _Op, typename... _Args>
  using detected_t
    = typename __detector<nonesuch, void, _Op, _Args...>::type;

template<typename _Default, template<typename...> class _Op, typename... _Args>
  using detected_or = __detected_or<_Default, _Op, _Args...>;

template<typename _Default, template<typename...> class _Op, typename... _Args>
  using detected_or_t = typename detected_or<_Default, _Op, _Args...>::type;

template<typename Expected, template<typename...> class _Op, typename... _Args>
  using is_detected_exact = std::is_same<Expected, detected_t<_Op, _Args...>>;

template<typename Expected, template<typename...> class _Op, typename... _Args>
  constexpr bool is_detected_exact_v
    = is_detected_exact<Expected, _Op, _Args...>::value;

template<typename _To, template<typename...> class _Op, typename... _Args>
  using is_detected_convertible
    = std::is_convertible<detected_t<_Op, _Args...>, _To>;

template<typename _To, template<typename...> class _Op, typename... _Args>
  constexpr bool is_detected_convertible_v
    = is_detected_convertible<_To, _Op, _Args...>::value;

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
