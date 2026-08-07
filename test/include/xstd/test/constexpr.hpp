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

// A digits loop of this header's own, rather than a call to xstd::to_chars.
// Two reasons, and either alone would be enough:
//
// - This renders a value into a *failure* message. A diagnostic implemented
//   with the code under test reports nothing trustworthy about the run where
//   that code is what broke, and to_chars is itself one of the things these
//   tests check.
// - Boost.Test odr-uses print_log_value in every translation unit that
//   compares such a value, so the printer's body - and everything it
//   instantiates - is emitted into roughly twenty test binaries and called in
//   none of them, since it only runs when an assertion fails. Those unexecuted
//   copies are what a coverage gate counting each instantiation separately
//   sees, and no test can reach them without failing on purpose.
//
// Correctness only has to be enough for a diagnostic: decimal, and no
// pretense of the standard's interface. The magnitude is still never formed,
// because negating the minimum value has no representation.
template<integral_like T>
[[nodiscard]] auto to_decimal(T const value)
        -> std::string
{
        auto digits = std::string{};
        for (auto rest = value;;) {
                auto const digit = static_cast<int>(rest % T{10});
                digits.insert(digits.begin(), static_cast<char>('0' + (digit < 0 ? -digit : digit)));
                rest = static_cast<T>(rest / T{10});
                if (rest == T{0}) {
                        break;
                }
        }
        if constexpr (is_signed_like_v<T>) {
                if (value < T{0}) {
                        digits.insert(digits.begin(), '-');
                }
        }
        return digits;
}

} // namespace xstd::test

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

// Partially specialized rather than routed through std::format, so that a
// failing BOOST_CHECK_EQUAL on a div_t prints for every element type div_t
// accepts, not only those the standard library can format.
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
