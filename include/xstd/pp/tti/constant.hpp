#pragma once
#include <xstd/type_traits.hpp> // detected_or_t
#include <type_traits>          // integral_constant, remove_const

#define XSTD_PP_TTI_CONSTANT(NAME, DEFAULT)                                                             \
                                                                                                        \
template<class T>                                                                                       \
using NAME ## _constant = std::integral_constant<std::remove_const_t<decltype(T::NAME)>, T::NAME>;      \
                                                                                                        \
using NAME ## _default = std::integral_constant<decltype(DEFAULT), DEFAULT>;                            \
                                                                                                        \
template<class T>                                                                                       \
constexpr auto NAME ## _v = xstd::detected_or_t<NAME ## _default, NAME ## _constant, T>::value;         \

