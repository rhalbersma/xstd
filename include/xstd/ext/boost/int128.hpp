//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_EXT_BOOST_INT128_HPP
#define XSTD_EXT_BOOST_INT128_HPP

#include <xstd/type_traits/make_signed.hpp>    // make_signed
#include <xstd/type_traits/make_unsigned.hpp>  // make_unsigned
#include <boost/int128.hpp>                    // IWYU pragma: export; int128, uint128
#include <boost/int128/detail/int128_imp.hpp>  // IWYU pragma: export
#include <boost/int128/detail/uint128_imp.hpp> // IWYU pragma: export
#include <type_traits>                         // type_identity

// The facade declares neither type, and no public header of Boost's re-exports the two that do,
// so a consumer's include-cleaner is told its pair has no provider unless this header names them.
// Delete the two detail includes the day Boost.Int128 carries the pragmas, as Abseil already does.

// Boost.Int128 introduces the pair and cannot name xstd's traits, so the associations are made here.
namespace xstd {

// Spelled without the _t suffix upstream dropped after its last release tag.
template<>
struct make_unsigned<boost::int128::int128> : std::type_identity<boost::int128::uint128>
{};

template<>
struct make_signed<boost::int128::uint128> : std::type_identity<boost::int128::int128>
{};

} // namespace xstd

#endif // XSTD_EXT_BOOST_INT128_HPP
