#pragma once
#include <xstd/type_traits.hpp> // always_false_v

#define XSTD_PP_TTI_MISSING_TYPE(NAME)                  \
                                                        \
template<class T>                                       \
struct missing_type_ ## NAME                            \
{                                                       \
        static_assert(                                  \
                xstd::always_false_v<T>                 \
                "Error: missing type '" #NAME "'."      \
        );                                              \
};                                                      \
