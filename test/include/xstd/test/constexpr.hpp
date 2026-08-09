//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_CONSTEXPR_HPP
#define XSTD_TEST_CONSTEXPR_HPP

#include <xstd/concepts/integral_like.hpp>        // integral_like
#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like
#include <xstd/cstdint.hpp>                       // int128, uint128
#include <xstd/cstdlib/div_t.hpp>                 // div_t
#include <xstd/type_traits/is_signed_like.hpp>    // is_signed_like_v
#include <boost/test/unit_test.hpp>               // BOOST_CHECK, BOOST_CHECK_EQUAL
#include <ostream>                                // ostream
#include <string>                                 // string

namespace xstd::test {

// Its own digits loop rather than xstd::to_chars, for two reasons. A diagnostic
// written with the code under test says nothing about the run that code broke;
// and a printer runs only when a check fails, so calling into include/ from here
// would leave an instantiation of to_chars that no passing run ever executes.
// The coverage gate counts those per translation unit, and the ones that would
// go uncovered are real: CStdLib/Abs and CStdLib/UnsignedAbs name a 128-bit type
// in a BOOST_CHECK_EQUAL and call nothing else in that header. Kept under test/,
// which --exclude 'test/.*' takes out of the gate, this loop raises neither.
//
// Its answer is not taken on trust: CharConvToChars/HexBoundariesMatchGroundTruth
// pins the same seven values it renders here against strings computed rather than
// rendered, at every exact width the suite carries.
template<integral_like I>
[[nodiscard]] auto to_decimal(I const value)
        -> std::string
{
        auto digits = std::string{};
        for (auto rest = value;;) {
                auto const digit = static_cast<int>(rest % I{10});
                digits.insert(digits.begin(), static_cast<char>('0' + (digit < 0 ? -digit : digit)));
                rest = static_cast<I>(rest / I{10});
                if (rest == I{0}) {
                        break;
                }
        }
        if constexpr (is_signed_like_v<I>) {
                if (value < I{0}) {
                        digits.insert(digits.begin(), '-');
                }
        }
        return digits;
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
                ostr << xstd::test::to_decimal(value);
        }
};

template<>
struct print_log_value<xstd::uint128>
{
        auto operator()(std::ostream& ostr, xstd::uint128 const value) const
                -> void
        {
                ostr << xstd::test::to_decimal(value);
        }
};

// Specialized rather than routed through std::format, so a failing check on a
// div_t prints for every element type div_t accepts.
template<xstd::signed_integral_like S>
struct print_log_value<xstd::div_t<S>>
{
        auto operator()(std::ostream& ostr, xstd::div_t<S> const& d) const
                -> void
        {
                ostr << '(' << xstd::test::to_decimal(d.quot) << ", " << xstd::test::to_decimal(d.rem) << ')';
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
