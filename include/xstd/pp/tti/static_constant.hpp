#pragma once
#include <xstd/pp/tti/detail/box_static_constant.hpp>   // XSTD_PP_TTI_BOX_STATIC_CONSTANT
#include <xstd/pp/tti/detail/has_static_constant.hpp>   // XSTD_PP_TTI_HAS_STATIC_CONSTANT
#include <type_traits>	                                // conditional_t, integral_constant

#define XSTD_PP_TTI_STATIC_CONSTANT(NAME, DEFAULT)              \
                                                                \
XSTD_PP_TTI_HAS_STATIC_CONSTANT(NAME)                           \
XSTD_PP_TTI_BOX_STATIC_CONSTANT(NAME)                           \
                                                                \
template<class T>                                               \
using NAME ## _t = typename std::conditional_t<                 \
        has_static_constant_ ## NAME ## _v<T>,                  \
        box_static_constant_ ## NAME <T>,                       \
        std::integral_constant<decltype(DEFAULT), DEFAULT>      \
>::type;                                                        \
                                                                \
template<class T>                                               \
constexpr auto NAME ## _v = NAME ## _t<T>::value;               \
