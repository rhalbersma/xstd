#pragma once
#include <cstddef>      // size_t, ptrdiff_t

namespace xstd {
inline namespace literals {
inline namespace support_literals {

constexpr std::size_t operator "" _z(unsigned long long u)
{
        return static_cast<std::size_t>(u);
}

constexpr std::ptrdiff_t operator "" _t(unsigned long long u)
{
        return static_cast<std::ptrdiff_t>(u);
}

}       // inline namespace support_literals
}       // inline namespace literals
}       // namespace xstd
