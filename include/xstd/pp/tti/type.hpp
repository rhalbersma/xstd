#pragma once
#include <xstd/type_traits.hpp> // is_detected, is_detected_v, detected_or_t

#define XSTD_PP_TTI_TYPE(NAME, DEFAULT)                                                 \
                                                                                        \
template<class T>                                                                       \
using typename_ ## NAME ## _t = typename T::NAME;                                       \
                                                                                        \
template<class T>                                                                       \
using is_ ## NAME = xstd::is_detected<typename_ ## NAME ## _t, T>;                      \
                                                                                        \
template<class T>                                                                       \
constexpr auto is_ ## NAME ## _v = xstd::is_detected_v<typename_ ## NAME ## _t, T>;     \
                                                                                        \
template<class T>                                                                       \
using NAME ## _t = xstd::detected_or_t<DEFAULT, typename_ ## NAME ## _t, T>;            \
