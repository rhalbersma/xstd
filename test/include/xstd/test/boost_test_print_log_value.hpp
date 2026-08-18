//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_BOOST_TEST_PRINT_LOG_VALUE_HPP
#define XSTD_TEST_BOOST_TEST_PRINT_LOG_VALUE_HPP

#include <xstd/charconv/to_chars.hpp>     // to_chars, to_chars_max_size
#include <xstd/concepts/integer_like.hpp> // integer_like
#include <xstd/cstdint.hpp>               // int128, uint128
#include <xstd/cstdlib/div_t.hpp>         // div_t
#include <boost/test/unit_test.hpp>       // print_log_value
#include <array>                          // array
#include <cassert>                        // assert
#include <ostream>                        // ostream
#include <string_view>                    // string_view
#include <system_error>                   // errc

namespace xstd::test {

template<integer_like I>
auto print_integer_like(std::ostream& ostr, I const value) -> void
{
        auto buffer = std::array<char, to_chars_max_size<I>>{};
        auto const result = to_chars(buffer.data(), buffer.data() + buffer.size(), value);
        assert(result.ec == std::errc{});
        ostr << std::string_view(buffer.data(), result.ptr);
}

} // namespace xstd::test

namespace boost::test_tools::tt_detail {

template<xstd::integer_like I>
struct print_log_value<I>
{
        auto operator()(std::ostream& ostr, I const value) const -> void
        {
                xstd::test::print_integer_like(ostr, value);
        }
};

template<xstd::integer_like I>
struct print_log_value<xstd::div_t<I>>
{
        auto operator()(std::ostream& ostr, xstd::div_t<I> const& d) const -> void
        {
                ostr << '(';
                xstd::test::print_integer_like(ostr, d.quot);
                ostr << ", ";
                xstd::test::print_integer_like(ostr, d.rem);
                ostr << ')';
        }
};

} // namespace boost::test_tools::tt_detail

#endif // XSTD_TEST_BOOST_TEST_PRINT_LOG_VALUE_HPP
