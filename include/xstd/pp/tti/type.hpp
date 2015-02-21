#pragma once
#include <xstd/pp/tti/detail/box_type.hpp>      // XSTD_PP_TTI_BOX_TYPE
#include <xstd/pp/tti/detail/has_type.hpp>      // XSTD_PP_TTI_HAS_TYPE
#include <xstd/type_traits.hpp>                 // type_is
#include <type_traits>                          // conditional

#define XSTD_PP_TTI_TYPE(NAME, DEFAULT)                 \
                                                        \
XSTD_PP_TTI_HAS_TYPE(NAME)                              \
XSTD_PP_TTI_BOX_TYPE(NAME)                              \
                                                        \
template<class T>                                       \
using NAME ## _t = typename std::conditional_t<         \
        has_type_ ## NAME ## _v<T>,                     \
        box_type_ ## NAME <T>,                          \
        xstd::type_is<DEFAULT>                          \
>::type;                                                \
                                                        \
template<class T>                                       \
constexpr auto NAME ## _v = NAME ## _t<T>::value;       \
