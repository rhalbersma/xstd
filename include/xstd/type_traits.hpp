#pragma once

//          Copyright Rein Halbersma 2014-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>  // is_same_v

namespace xstd {

template<class T, class... Args>
inline constexpr auto any_of = (std::is_same_v<T, Args> || ...);

}       // namespace xstd
