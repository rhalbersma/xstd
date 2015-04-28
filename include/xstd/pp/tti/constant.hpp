#pragma once
#include <xstd/type_traits.hpp> // is_detected, is_detected_v, detected_or_t
#include <type_traits>          // integral_constant, remove_const

#define XSTD_PP_TTI_CONSTANT(NAME, DEFAULT)                                                                             \
                                                                                                                        \
template<class T>                                                                                                       \
using constant_ ## NAME = std::integral_constant<std::remove_const_t<decltype(T::NAME)>, T::NAME>;                      \
                                                                                                                        \
template<class T>                                                                                                       \
using is_ ## NAME = xstd::is_detected<constant_ ## NAME, T>;                                                            \
                                                                                                                        \
template<class T>                                                                                                       \
constexpr auto is_ ## NAME ## _v = xstd::is_detected_v<constant_ ## NAME, T>;                                           \
                                                                                                                        \
template<class T>                                                                                                       \
using NAME ## _t = xstd::detected_or_t<std::integral_constant<decltype(DEFAULT), DEFAULT>, constant_ ## NAME, T>;       \
                                                                                                                        \
template<class T>                                                                                                       \
constexpr auto NAME ## _v = NAME ## _t<T>::value;                                                                       \
