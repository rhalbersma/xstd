#pragma once
#include <cassert>      // assert
#include <cstddef>      // size_t
#include <limits>       // digits, is_signed, is_integer, max
#include <type_traits>  // bool_constant (C++1z)

namespace xstd {

template<class T>
constexpr auto is_unsigned_integer =
        !std::numeric_limits<T>::is_signed &&
         std::numeric_limits<T>::is_integer
;

template<class T>
constexpr auto digits = static_cast<std::size_t>(std::numeric_limits<T>::digits);

template<class Numerator, class Denominator>
constexpr auto digits_ratio = digits<Numerator> / digits<Denominator>;

template<class T, std::size_t N>
constexpr auto is_representable_v = N <= (std::numeric_limits<T>::max() + 1);

template<class T, std::size_t N>
using is_representable_t = std::bool_constant<is_representable_v<T, N>>;

template<class T>
constexpr auto is_power_of_2(T x) noexcept
{
        static_assert(is_unsigned_integer<T>, "");
        return (x - 1) < (x & - x);
}

constexpr auto align_on(std::size_t offset, std::size_t align) noexcept
{
        assert(is_power_of_2(align));
        return (offset + align - 1) & ~(align - 1);
}

}       // namespace xstd
