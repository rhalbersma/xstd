#pragma once
#include <xstd/pp/tti/detail/box_type.hpp>      // XSTD_PP_TTI_BOX_TYPE
#include <xstd/pp/tti/detail/has_type.hpp>      // XSTD_PP_TTI_HAS_TYPE
#include <xstd/pp/tti/detail/missing_type.hpp>  // XSTD_PP_TTI_MISSING_TYPE
#include <type_traits>                          // conditional_t

#define XSTD_PP_TTI_TYPE_NO_DEFAULT(NAME)               \
                                                        \
XSTD_PP_TTI_HAS_TYPE(NAME)                              \
XSTD_PP_TTI_BOX_TYPE(NAME)                              \
XSTD_PP_TTI_MISSING_TYPE(NAME)                          \
                                                        \
template<class T>                                       \
using NAME ## _t = typename std::conditional_t<         \
        has_type_ ## NAME ## _v<T>,                     \
        box_type_ ## NAME <T>,                          \
        missing_type_ ## NAME <T>                       \
>::type;                                                \
                                                        \
template<class T>                                       \
constexpr auto NAME ## _v = NAME ## _t<T>::value;       \
