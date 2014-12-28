#pragma once
#include <xstd/type_traits.hpp> // always_false_v

#define XSTD_PP_TTI_MISSING_STATIC_CONSTANT(NAME)               \
                                                                \
template<class T>                                               \
struct missing_static_constant_ ## NAME                         \
{                                                               \
        static_assert(                                          \
                xstd::always_false_v<T>,                        \
                "Error: missing static constant '" #NAME "'."   \
        );                                                      \
};                                                              \
