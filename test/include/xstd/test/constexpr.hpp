//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_CONSTEXPR_HPP
#define XSTD_TEST_CONSTEXPR_HPP

#include <xstd/charconv/to_chars.hpp>     // to_chars, to_chars_max_size
#include <xstd/concepts/integer_like.hpp> // integer_like
#include <xstd/cstdint.hpp>               // int128, uint128
#include <xstd/cstdlib/div_t.hpp>         // div_t
#include <boost/test/unit_test.hpp>       // BOOST_CHECK, BOOST_CHECK_EQUAL
#include <array>                          // array
#include <cassert>                        // assert
#include <ostream>                        // ostream
#include <string_view>                    // string_view
#include <system_error>                   // errc

namespace xstd::test {

// The one thing that renders a 128-bit value everywhere the suite builds.
template<integer_like I>
auto print_integer_like(std::ostream& ostr, I const value)
        -> void
{
        auto buffer = std::array<char, to_chars_max_size<I>>{};
        auto const result = to_chars(buffer.data(), buffer.data() + buffer.size(), value);
        // The buffer is sized for base 2, so decimal always fits.
        assert(result.ec == std::errc{});
        ostr << std::string_view(buffer.data(), result.ptr);
}

} // namespace xstd::test

// How Boost.Test asks for a value, there being nowhere legal to put an inserter.
namespace boost::test_tools::tt_detail {

template<>
struct print_log_value<xstd::int128>
{
        auto operator()(std::ostream& ostr, xstd::int128 const value) const
                -> void
        {
                xstd::test::print_integer_like(ostr, value);
        }
};

template<>
struct print_log_value<xstd::uint128>
{
        auto operator()(std::ostream& ostr, xstd::uint128 const value) const
                -> void
        {
                xstd::test::print_integer_like(ostr, value);
        }
};

// Specialized, not routed through std::format, so it prints for every element type.
template<xstd::integer_like I>
struct print_log_value<xstd::div_t<I>>
{
        auto operator()(std::ostream& ostr, xstd::div_t<I> const& d) const
                -> void
        {
                ostr << '(';
                xstd::test::print_integer_like(ostr, d.quot);
                ostr << ", ";
                xstd::test::print_integer_like(ostr, d.rem);
                ostr << ')';
        }
};

} // namespace boost::test_tools::tt_detail

// No STATIC_REQUIRE in Boost.Test, so one expression becomes both checks and cannot drift.
#define XSTD_CONSTEXPR_CHECK(...) \
        static_assert(__VA_ARGS__); \
        BOOST_CHECK(__VA_ARGS__)

#define XSTD_CONSTEXPR_CHECK_EQUAL(a, b) \
        static_assert((a) == (b)); \
        BOOST_CHECK_EQUAL((a), (b))

#endif // XSTD_TEST_CONSTEXPR_HPP
