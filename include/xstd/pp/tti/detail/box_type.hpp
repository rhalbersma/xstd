#pragma once

#define XSTD_PP_TTI_BOX_TYPE(NAME)      \
                                        \
template<class T>                       \
struct box_type_ ## NAME                \
{                                       \
        using type = typename T::NAME;  \
};                                      \
