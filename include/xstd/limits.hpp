#pragma once
#include <cstddef>      // size_t
#include <limits>       // digits, is_signed, is_integer

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

}       // namespace xstd
