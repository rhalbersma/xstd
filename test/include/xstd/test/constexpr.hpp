//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_CONSTEXPR_HPP
#define XSTD_TEST_CONSTEXPR_HPP

#include <xstd/cstdint.hpp>         // int128_t, uint128_t
#include <xstd/format/int128.hpp>   // detail::to_decimal
#include <boost/test/unit_test.hpp> // BOOST_CHECK, BOOST_CHECK_EQUAL
#include <ostream>                  // ostream

namespace boost::test_tools::tt_detail {

template<>
struct print_log_value<xstd::int128_t>
{
        auto operator()(std::ostream& ostr, xstd::int128_t const value) const
                -> void
        {
                ostr << xstd::detail::to_decimal(value);
        }
};

template<>
struct print_log_value<xstd::uint128_t>
{
        auto operator()(std::ostream& ostr, xstd::uint128_t const value) const
                -> void
        {
                ostr << xstd::detail::to_decimal(value);
        }
};

} // namespace boost::test_tools::tt_detail

// Boost.Test has no STATIC_REQUIRE-style construct, so a check that a
// constexpr function is actually usable in a constant expression is easy to
// forget: static_assert and BOOST_CHECK*, written separately, drift apart or
// get left as runtime-only. These macros expand an expression written once
// into both a static_assert (constexpr-evaluability) and the matching
// runtime check (Boost.Test's usual pass/fail reporting), so there is a
// single source of truth for the checked value.
#define XSTD_CONSTEXPR_CHECK(...) \
        static_assert(__VA_ARGS__); \
        BOOST_CHECK(__VA_ARGS__)

#define XSTD_CONSTEXPR_CHECK_EQUAL(a, b) \
        static_assert((a) == (b)); \
        BOOST_CHECK_EQUAL((a), (b))

#endif // XSTD_TEST_CONSTEXPR_HPP
