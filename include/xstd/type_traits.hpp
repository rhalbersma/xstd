#pragma once
#include <type_traits>  // underlying_type

namespace xstd {

template<class T>
struct type_is
{
        using type = T;
};

template<class E>
constexpr auto to_underlying_type(E const e) noexcept
{
        static_assert(std::is_enum<E>{});
        return static_cast<std::underlying_type_t<E>>(e);
}

}       // namespace xstd
