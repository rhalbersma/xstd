#pragma once
#include <cstddef>      // ptrdiff_t, size_t

namespace xstd {
inline namespace literals {
inline namespace support_literals {

constexpr std::ptrdiff_t operator"" _t(unsigned long long u)
{
        return static_cast<std::ptrdiff_t>(u);
}

constexpr std::size_t operator"" _z(unsigned long long u)
{
        return static_cast<std::size_t>(u);
}

}       // inline namespace support_literals
}       // inline namespace literals
}       // namespace xstd
