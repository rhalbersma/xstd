#pragma once

namespace xstd {

// www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3911.pdf
template<class...>
using void_t = void;

template<class T>
struct type_is
{
        using type = T;
};

}       // namespace xstd
