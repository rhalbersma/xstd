//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CSTDINT_HPP
#define XSTD_CSTDINT_HPP

#include <xstd/config/int128.hpp>             // IWYU pragma: export; int128, uint128
#include <xstd/type_traits/make_signed.hpp>   // make_signed
#include <xstd/type_traits/make_unsigned.hpp> // make_unsigned
#include <type_traits>                        // type_identity

// The pair's associations, where the pair is introduced: whoever can name it has them.
namespace xstd {

template<>
struct make_unsigned<int128> : std::type_identity<uint128>
{};

template<>
struct make_signed<uint128> : std::type_identity<int128>
{};

} // namespace xstd

#endif // XSTD_CSTDINT_HPP
