//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_CONSTEXPR_HPP
#define XSTD_TEST_CONSTEXPR_HPP

#include <xstd/charconv/to_chars.hpp>             // to_chars, to_chars_max_size
#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like
#include <xstd/cstdint.hpp>                       // int128, uint128
#include <xstd/cstdlib/div_t.hpp>                 // div_t
#include <boost/test/unit_test.hpp>               // BOOST_CHECK, BOOST_CHECK_EQUAL
#include <array>                                  // array
#include <ostream>                                // ostream
#include <string_view>                            // string_view

// Boost.Test prints a value only when an assertion fails, and it asks for that
// printing through print_log_value rather than through operator<< directly.
// Specializing it is what lets xstd report 128-bit values and div_t without
// providing stream inserters of its own - which for the 128-bit types it could
// not do anyway. They are either built-ins, whose associated namespace set is
// empty so an xstd::operator<< is unreachable by ADL, or standard-library
// types, whose associated namespace is one no program may add to.
namespace boost::test_tools::tt_detail {

template<>
struct print_log_value<xstd::int128>
{
        auto operator()(std::ostream& ostr, xstd::int128 const value) const
                -> void
        {
                constexpr auto N = xstd::to_chars_max_size<xstd::int128>;
                auto buffer = std::array<char, N>{};
                auto const result = xstd::to_chars(buffer.data(), buffer.data() + N, value);
                ostr << std::string_view{buffer.data(), result.ptr};
        }
};

template<>
struct print_log_value<xstd::uint128>
{
        auto operator()(std::ostream& ostr, xstd::uint128 const value) const
                -> void
        {
                constexpr auto N = xstd::to_chars_max_size<xstd::uint128>;
                auto buffer = std::array<char, N>{};
                auto const result = xstd::to_chars(buffer.data(), buffer.data() + N, value);
                ostr << std::string_view{buffer.data(), result.ptr};
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
                // A buffer each: the two views are alive at the same time, so
                // sharing one would leave the first dangling into storage the
                // second call had already rewritten.
                constexpr auto N = xstd::to_chars_max_size<S>;
                auto quot = std::array<char, N>{};
                auto rem = std::array<char, N>{};
                auto const render = [](std::array<char, N>& buffer, S const value) {
                        auto const result = xstd::to_chars(buffer.data(), buffer.data() + N, value);
                        return std::string_view{buffer.data(), result.ptr};
                };
                ostr << '(' << render(quot, d.quot) << ", " << render(rem, d.rem) << ')';
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
