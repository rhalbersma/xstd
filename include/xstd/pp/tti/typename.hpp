#pragma once
#include <xstd/type_traits.hpp> // is_detected_v, detected_or_t

#define XSTD_PP_TTI_TYPENAME(NAME, DEFAULT)                             \
                                                                        \
template<class T>                                                       \
using NAME ## _t = typename T::NAME ## _type;                           \
                                                                        \
template<class T>                                                       \
constexpr auto is_ ## NAME ## _v = xstd::is_detected_v<NAME ## _t, T>;  \
                                                                        \
template<class T>                                                       \
using NAME ## _or_t = xstd::detected_or_t<DEFAULT, NAME ## _t, T>;      \

