//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_CONSTEXPR_HPP
#define XSTD_TEST_CONSTEXPR_HPP

#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like
#include <xstd/cstdint.hpp>                       // int128_t, uint128_t
#include <xstd/cstdlib/div_t.hpp>                 // div_t
#include <xstd/format/int128.hpp>                 // detail::decimal_buffer_size, detail::to_decimal
#include <boost/test/unit_test.hpp>               // BOOST_CHECK, BOOST_CHECK_EQUAL
#include <ostream>                                // ostream

// Boost.Test prints a value only when an assertion fails, and it asks for that
// printing through print_log_value rather than through operator<< directly.
// Specializing it is what lets xstd report 128-bit values and div_t without
// providing stream inserters of its own - which for the 128-bit types it could
// not do anyway. They are either built-ins, whose associated namespace set is
// empty so an xstd::operator<< is unreachable by ADL, or standard-library
// types, whose associated namespace is one no program may add to.
namespace boost::test_tools::tt_detail {

template<>
struct print_log_value<xstd::int128_t>
{
        auto operator()(std::ostream& ostr, xstd::int128_t const value) const
                -> void
        {
                char buffer[xstd::detail::decimal_buffer_size<xstd::int128_t>];
                ostr << xstd::detail::to_decimal(buffer, value);
        }
};

template<>
struct print_log_value<xstd::uint128_t>
{
        auto operator()(std::ostream& ostr, xstd::uint128_t const value) const
                -> void
        {
                char buffer[xstd::detail::decimal_buffer_size<xstd::uint128_t>];
                ostr << xstd::detail::to_decimal(buffer, value);
        }
};

// Partially specialized rather than routed through std::format, so that a
// failing BOOST_CHECK_EQUAL on a div_t prints for every element type div_t
// accepts, not only those the standard library can format.
template<xstd::signed_integral_like S>
struct print_log_value<xstd::div_t<S>>
{
        auto operator()(std::ostream& ostr, xstd::div_t<S> const& d) const
                -> void
        {
                char quot[xstd::detail::decimal_buffer_size<S>];
                char rem[xstd::detail::decimal_buffer_size<S>];
                ostr << '(' << xstd::detail::to_decimal(quot, d.quot)
                     << ", " << xstd::detail::to_decimal(rem, d.rem) << ')';
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
