//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_BOOST_INT128_HPP
#define XSTD_TEST_BOOST_INT128_HPP

// An integer-class type xstd has never heard of, which xstd::int128 cannot be.
#if __has_include(<boost/int128.hpp>)

#define XSTD_TEST_HAS_BOOST_INT128

#include <boost/int128.hpp>                   // int128, uint128
#include <xstd/type_traits/make_signed.hpp>   // make_signed
#include <xstd/type_traits/make_unsigned.hpp> // make_unsigned
#include <type_traits>                        // type_identity

namespace xstd::test {

// Named once: upstream dropped the _t suffix after its last release tag.
using boost_int128 = boost::int128::int128;
using boost_uint128 = boost::int128::uint128;

} // namespace xstd::test

namespace xstd {

// The one thing a library cannot work out for a type it does not know.
template<>
struct make_unsigned<test::boost_int128> : std::type_identity<test::boost_uint128>
{};

template<>
struct make_signed<test::boost_uint128> : std::type_identity<test::boost_int128>
{};

} // namespace xstd

#endif // __has_include(<boost/int128.hpp>)

#endif // XSTD_TEST_BOOST_INT128_HPP
