#pragma once
#include <xstd/type_traits.hpp> // detected_or_t
#include <type_traits>          // integral_constant, remove_const

#define XSTD_PP_TTI_STATIC_CONSTANT(NAME, DEFAULT)                                                                              \
                                                                                                                                \
template<class T>                                                                                                               \
using static_constant_ ## NAME ## _t = std::integral_constant<std::remove_const_t<decltype(T::NAME)>, T::NAME>;                 \
                                                                                                                                \
template<class T>                                                                                                               \
using NAME ## _t = xstd::detected_or_t<std::integral_constant<decltype(DEFAULT), DEFAULT>, static_constant_ ## NAME ## _t, T>;  \
                                                                                                                                \
template<class T>                                                                                                               \
constexpr auto NAME ## _v = NAME ## _t<T>::value;                                                                               \
