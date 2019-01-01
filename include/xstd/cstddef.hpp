#pragma once

//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>      // size_t

namespace xstd {
inline namespace literals {
inline namespace support_literals {

constexpr auto operator "" _zu(unsigned long long u)
{
        return static_cast<std::size_t>(u);
}

}       // inline namespace support_literals
}       // inline namespace literals
}       // namespace xstd
