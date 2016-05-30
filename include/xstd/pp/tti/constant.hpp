#pragma once
#include <xstd/type_traits.hpp> // is_detected_v, detected_or_t
#include <type_traits>          // integral_constant, remove_const

#define XSTD_PP_TTI_CONSTANT(NAME, DEFAULT)                                                                             \
                                                                                                                        \
template<class T>                                                                                                       \
using NAME ## _constant = std::integral_constant<std::remove_const_t<decltype(T::NAME)>, T::NAME>;                      \
                                                                                                                        \
template<class T>                                                                                                       \
constexpr auto is_ ## NAME ## _v = xstd::is_detected_v<NAME ## _constant, T>;                                           \
                                                                                                                        \
template<class T>                                                                                                       \
using NAME ## _or_t = xstd::detected_or_t<std::integral_constant<decltype(DEFAULT), DEFAULT>, NAME ## _constant, T>;    \
                                                                                                                        \
template<class T>                                                                                                       \
constexpr auto NAME ## _or_v = NAME ## _or_t<T>::value;                                                                 \

