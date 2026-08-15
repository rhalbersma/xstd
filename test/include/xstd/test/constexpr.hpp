//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_CONSTEXPR_HPP
#define XSTD_TEST_CONSTEXPR_HPP

#include <xstd/charconv/to_chars.hpp>      // to_chars, to_chars_max_size
#include <xstd/concepts/integral_like.hpp> // integral_like
#include <xstd/cstdint.hpp>                // int128, uint128
#include <xstd/cstdlib/div_t.hpp>          // div_t
#include <boost/test/unit_test.hpp>        // BOOST_CHECK, BOOST_CHECK_EQUAL
#include <array>                           // array
#include <cassert>                         // assert
#include <ostream>                         // ostream
#include <string_view>                     // string_view
#include <system_error>                    // errc

namespace xstd::test {

// xstd::to_chars, the one thing that renders a 128-bit value everywhere the
// suite builds. The standard has nothing that does: neither spelling of
// xstd::int128 has an ostream inserter, and while std::to_chars and
// std::format both take __int128 on some standard libraries - libstdc++ added
// the strict-mode to_chars overloads after GCC 13, where the call is still
// ambiguous - neither takes the Microsoft STL's _Signed128, which declares no
// more than its shifts.
//
// Which makes this the code under test only where the standard covers
// nothing: elsewhere xstd::to_chars is a call to std::to_chars, by its own
// constrained overload. Sound even there, because to_chars is not on trial in
// the suites this printer serves, and the suite where it is on trial compares
// strings against literals and asks for no printer at all - so a fault in it
// is caught by its own checks rather than by garbling someone else's.
template<integral_like I>
auto print_integral_like(std::ostream& ostr, I const value)
        -> void
{
        auto buffer = std::array<char, to_chars_max_size<I>>{};
        auto const result = to_chars(buffer.data(), buffer.data() + buffer.size(), value);
        // The buffer is sized for base 2, so decimal always fits.
        assert(result.ec == std::errc{});
        ostr << std::string_view(buffer.data(), result.ptr);
}

} // namespace xstd::test

// How Boost.Test asks for a value to be printed, and what lets xstd report
// 128-bit values without stream inserters it has nowhere legal to put.
namespace boost::test_tools::tt_detail {

template<>
struct print_log_value<xstd::int128>
{
        auto operator()(std::ostream& ostr, xstd::int128 const value) const
                -> void
        {
                xstd::test::print_integral_like(ostr, value);
        }
};

template<>
struct print_log_value<xstd::uint128>
{
        auto operator()(std::ostream& ostr, xstd::uint128 const value) const
                -> void
        {
                xstd::test::print_integral_like(ostr, value);
        }
};

// Specialized rather than routed through std::format, so a failing check on a
// div_t prints for every element type div_t accepts.
template<xstd::integral_like I>
struct print_log_value<xstd::div_t<I>>
{
        auto operator()(std::ostream& ostr, xstd::div_t<I> const& d) const
                -> void
        {
                ostr << '(';
                xstd::test::print_integral_like(ostr, d.quot);
                ostr << ", ";
                xstd::test::print_integral_like(ostr, d.rem);
                ostr << ')';
        }
};

} // namespace boost::test_tools::tt_detail

// Boost.Test has no STATIC_REQUIRE, so these expand one expression into both a
// static_assert and the matching runtime check, which cannot then drift apart.
#define XSTD_CONSTEXPR_CHECK(...) \
        static_assert(__VA_ARGS__); \
        BOOST_CHECK(__VA_ARGS__)

#define XSTD_CONSTEXPR_CHECK_EQUAL(a, b) \
        static_assert((a) == (b)); \
        BOOST_CHECK_EQUAL((a), (b))

#endif // XSTD_TEST_CONSTEXPR_HPP
