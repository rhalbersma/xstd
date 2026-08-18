//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_BOOST_TEST_PRINT_LOG_VALUE_HPP
#define XSTD_TEST_BOOST_TEST_PRINT_LOG_VALUE_HPP

#include <xstd/charconv/to_chars.hpp>     // to_chars, to_chars_max_size
#include <xstd/concepts/integer.hpp> // integer
#include <xstd/cstdint.hpp>               // int128, uint128
#include <xstd/cstdlib/div_result.hpp>         // div_result
#include <boost/test/unit_test.hpp>       // print_log_value
#ifdef __BITINT_MAXWIDTH__
#include <xstd/test/bit_integer.hpp> // bit_integer
#endif
#include <array>        // array
#include <cassert>      // assert
#include <ostream>      // ostream
#include <string_view>  // string_view
#include <system_error> // errc

namespace xstd::test {

template<integer I>
auto print_integer(std::ostream& ostr, I const value) -> void
{
        auto buffer = std::array<char, to_chars_max_size<I>>{};
        auto const result = to_chars(buffer.data(), buffer.data() + buffer.size(), value);
        assert(result.ec == std::errc{});
        ostr << std::string_view(buffer.data(), result.ptr);
}

} // namespace xstd::test

namespace boost::test_tools::tt_detail {

template<>
struct print_log_value<xstd::int128>
{
        auto operator()(std::ostream& ostr, xstd::int128 const value) const -> void
        {
                xstd::test::print_integer(ostr, value);
        }
};

template<>
struct print_log_value<xstd::uint128>
{
        auto operator()(std::ostream& ostr, xstd::uint128 const value) const -> void
        {
                xstd::test::print_integer(ostr, value);
        }
};

#ifdef __BITINT_MAXWIDTH__

template<class Storage>
struct print_log_value<xstd::test::bit_integer<Storage>>
{
        auto operator()(std::ostream& ostr, xstd::test::bit_integer<Storage> const value) const -> void
        {
                xstd::test::print_integer(ostr, value);
        }
};

#endif // __BITINT_MAXWIDTH__

template<xstd::integer I>
struct print_log_value<xstd::div_result<I>>
{
        auto operator()(std::ostream& ostr, xstd::div_result<I> const& d) const -> void
        {
                ostr << '(';
                xstd::test::print_integer(ostr, d.quotient);
                ostr << ", ";
                xstd::test::print_integer(ostr, d.remainder);
                ostr << ')';
        }
};

} // namespace boost::test_tools::tt_detail

#endif // XSTD_TEST_BOOST_TEST_PRINT_LOG_VALUE_HPP
