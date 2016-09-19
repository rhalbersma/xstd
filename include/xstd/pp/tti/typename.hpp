#pragma once
#include <xstd/type_traits.hpp> // detected_or_t

#define XSTD_PP_TTI_TYPENAME(NAME, DEFAULT)                                     \
                                                                                \
template<class T>                                                               \
using NAME ## _typename = typename T::NAME ## _type;                            \
                                                                                \
using NAME ## _default = DEFAULT;                                               \
                                                                                \
template<class T>                                                               \
using NAME ## _t = xstd::detected_or_t<NAME ## _default, NAME ## _typename, T>; \

