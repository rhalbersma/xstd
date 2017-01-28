#pragma once
#include <cstddef>      // size_t

namespace xstd {
inline namespace literals {
inline namespace support_literals {

constexpr std::size_t operator "" _zu(unsigned long long u)
{
        return static_cast<std::size_t>(u);
}

}       // inline namespace support_literals
}       // inline namespace literals

using namespace literals::support_literals;

}       // namespace xstd
