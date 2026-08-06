//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib.hpp>         // complete arithmetic surface
#include <xstd/cstdint.hpp>         // int128, uint128
#include <xstd/concepts.hpp>        // signed_integral_like
#include <xstd/type_traits.hpp>     // make_unsigned_like_t
#include <xstd/test/constexpr.hpp>  // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp> // Boost.Test
#include <algorithm>                // ranges::transform
#include <array>                    // array
#include <concepts>                 // integral, same_as, signed_integral
#include <cstdint>                  // exact-width integer types, intmax_t
#include <cstdlib>                  // div
#include <iterator>                 // back_inserter
#include <limits>                   // numeric_limits
#include <tuple>                    // tuple
#include <type_traits>              // make_unsigned_t
#include <utility>                  // pair
#include <vector>                   // vector

BOOST_AUTO_TEST_SUITE(CStdLib)

// The exact-width signed types, which is what one constrained template buys
// over the <cstdlib>-style abs/labs/llabs/imaxabs naming: int8_t and int16_t
// have no name in that scheme at all, and the two that do (int32_t, int64_t)
// no longer need one. The portable 128-bit aliases get their own case below.
using exact_width_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;

// Named concepts rather than bare requires-expressions in the test bodies:
// a requires-expression whose operand is invalid *and* non-dependent is a
// hard error on GCC, so the type has to stay a template parameter.
template<class T>
concept has_abs = requires (T x) { xstd::abs(x); };

template<class T>
concept has_unsigned_abs = requires (T x) { xstd::unsigned_abs(x); };

template<class T>
concept has_sign = requires (T x) { xstd::sign(x); };

template<class T, class U>
concept has_div = requires (T numer, U denom) { xstd::div(numer, denom); };

// The constraint itself: signed integral in, nothing else, and the argument
// type comes back out rather than the promoted type.
BOOST_AUTO_TEST_CASE_TEMPLATE(Constraints, T, exact_width_types)
{
        static_assert(std::same_as<decltype(xstd::abs(T{})), T>);
        static_assert(std::same_as<decltype(xstd::unsigned_abs(T{})), std::make_unsigned_t<T>>);
        static_assert(std::same_as<decltype(xstd::sign(T{})), int>);
        static_assert(std::same_as<decltype(xstd::div(T{1}, T{1})), xstd::div_t<T>>);
        static_assert(std::same_as<decltype(xstd::euclidean_div(T{1}, T{1})), xstd::div_t<T>>);
        static_assert(std::same_as<decltype(xstd::floored_div(T{1}, T{1})), xstd::div_t<T>>);

        // The integer interface and every facility built on it are noexcept.
        static_assert(noexcept(xstd::abs(T{})));
        static_assert(noexcept(xstd::unsigned_abs(T{})));
        static_assert(noexcept(xstd::sign(T{})));
        static_assert(noexcept(xstd::div(T{1}, T{1})));
        static_assert(noexcept(xstd::euclidean_div(T{1}, T{1})));
        static_assert(noexcept(xstd::floored_div(T{1}, T{1})));

        static_assert(has_abs<T>);
        static_assert(has_unsigned_abs<T>);
        static_assert(has_sign<T>);
        static_assert(has_div<T, T>);

        // Unsigned arguments are outside the constraint, at every width.
        using U = std::make_unsigned_t<T>;
        static_assert(not has_abs<U>);
        static_assert(not has_unsigned_abs<U>);
        static_assert(not has_sign<U>);
        static_assert(not has_div<U, U>);

        BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
}

BOOST_AUTO_TEST_CASE(NonSignedIntegralArgumentsAreRejected)
{
        static_assert(not has_abs<bool>);
        static_assert(not has_abs<double>);
        static_assert(not has_sign<char*>);

        // unsigned_abs is the only one of the three whose return type is computed by a
        // trait, so it is the only one where rejecting a non-integral argument
        // depends on the trait never being instantiated. Clang before 21 does
        // not implement CWG2369 and substitutes the return type before checking
        // the constraint, so spelling std::make_unsigned_t<T> in the signature
        // makes the double and char* cases a hard error rather than a
        // substitution failure - these two stop compiling instead of being
        // false. The deduced return type in the header is what keeps them
        // compiling; see doc/design.md.
        static_assert(not has_unsigned_abs<bool>);
        static_assert(not has_unsigned_abs<double>);
        static_assert(not has_unsigned_abs<char*>);

        // Both parameters deduce the same T, so a mixed-width call is a
        // deduction failure rather than a silent conversion of one operand.
        static_assert(not has_div<std::int32_t, std::int64_t>);
        static_assert(std::same_as<decltype(xstd::div<std::int64_t>(8, 3)), xstd::div_t<std::int64_t>>);

        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ExactDivisions, T, exact_width_types)
{
        XSTD_CONSTEXPR_CHECK((xstd::div(T{+6}, T{+3}) == xstd::div_t<T>{+2, 0}));
        XSTD_CONSTEXPR_CHECK((xstd::euclidean_div(T{+6}, T{+3}) == xstd::div_t<T>{+2, 0}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{+6}, T{+3}) == xstd::div_t<T>{+2, 0}));

        XSTD_CONSTEXPR_CHECK((xstd::div(T{+6}, T{-3}) == xstd::div_t<T>{-2, 0}));
        XSTD_CONSTEXPR_CHECK((xstd::euclidean_div(T{+6}, T{-3}) == xstd::div_t<T>{-2, 0}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{+6}, T{-3}) == xstd::div_t<T>{-2, 0}));

        XSTD_CONSTEXPR_CHECK((xstd::div(T{-6}, T{+3}) == xstd::div_t<T>{-2, 0}));
        XSTD_CONSTEXPR_CHECK((xstd::euclidean_div(T{-6}, T{+3}) == xstd::div_t<T>{-2, 0}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-6}, T{+3}) == xstd::div_t<T>{-2, 0}));

        XSTD_CONSTEXPR_CHECK((xstd::div(T{-6}, T{-3}) == xstd::div_t<T>{+2, 0}));
        XSTD_CONSTEXPR_CHECK((xstd::euclidean_div(T{-6}, T{-3}) == xstd::div_t<T>{+2, 0}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-6}, T{-3}) == xstd::div_t<T>{+2, 0}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BoundaryDivisions, T, exact_width_types)
{
        using limits = std::numeric_limits<T>;
        constexpr auto min = limits::min();
        constexpr auto max = limits::max();

        XSTD_CONSTEXPR_CHECK((xstd::div(min, T{+1}) == xstd::div_t<T>{min, 0}));
        XSTD_CONSTEXPR_CHECK((xstd::euclidean_div(min, T{+1}) == xstd::div_t<T>{min, 0}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(min, T{+1}) == xstd::div_t<T>{min, 0}));

        // denom == MIN is in contract even though |MIN| is not representable
        // in T: it is exactly why the postconditions are written with unsigned_abs
        // rather than abs.
        XSTD_CONSTEXPR_CHECK((xstd::div(T{+1}, min) == xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK((xstd::euclidean_div(T{+1}, min) == xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{+1}, min) == xstd::div_t<T>{-1, static_cast<T>(min + 1)}));

        // denom == MIN again, but with a negative remainder, which is what
        // selects euclidean_div's negative adjustment. Computing that one as
        // rem + I * denom would form -MIN and overflow; being a constant
        // expression, this check catches that at compile time as well as
        // under a sanitizer. Promotion hides it below int, so the wider
        // instantiations of this case are the ones that matter.
        XSTD_CONSTEXPR_CHECK((xstd::div(T{-1}, min) == xstd::div_t<T>{0, -1}));
        XSTD_CONSTEXPR_CHECK((xstd::euclidean_div(T{-1}, min) == xstd::div_t<T>{+1, max}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-1}, min) == xstd::div_t<T>{0, -1}));

        XSTD_CONSTEXPR_CHECK((xstd::div(max, T{-1}) == xstd::div_t<T>{static_cast<T>(-max), 0}));
        XSTD_CONSTEXPR_CHECK((xstd::euclidean_div(max, T{-1}) == xstd::div_t<T>{static_cast<T>(-max), 0}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(max, T{-1}) == xstd::div_t<T>{static_cast<T>(-max), 0}));

        XSTD_CONSTEXPR_CHECK((xstd::div(min, max) == xstd::div_t<T>{-1, -1}));
        XSTD_CONSTEXPR_CHECK((xstd::euclidean_div(min, max) == xstd::div_t<T>{-2, static_cast<T>(max - 1)}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(min, max) == xstd::div_t<T>{-2, static_cast<T>(max - 1)}));
}

// The whole arithmetic surface at one type, exercising both arms of every
// conditional rather than calling each function once: a single call per type
// would leave euclidean_div's and floored_div's remainder adjustments
// half-covered.
//
// Constrained to xstd::signed_integral_like rather than std::signed_integral,
// so the same battery runs unchanged on a type the latter can never accept -
// which is the point of the widening, and a stronger check than a separate
// hand-written battery would be, since every expected value here was written
// for the built-in widths first.
//
// Written with XSTD_CONSTEXPR_CHECK rather than XSTD_CONSTEXPR_CHECK_EQUAL
// throughout: reporting a mismatched value needs a stream inserter, which an
// integer-class type is under no obligation to have. The exact-width test
// cases above keep the value-printing form, so a regression at a built-in
// width still reports both operands.
template<xstd::signed_integral_like T>
auto check_signed_integral_like()
        -> void
{
        using U = xstd::make_unsigned_like_t<T>;
        using limits = std::numeric_limits<T>;

        XSTD_CONSTEXPR_CHECK((xstd::abs(T{-2}) == T{2}));
        XSTD_CONSTEXPR_CHECK((xstd::abs(T{+2}) == T{2}));
        XSTD_CONSTEXPR_CHECK((xstd::abs(static_cast<T>(limits::min() + T{1})) == limits::max()));

        XSTD_CONSTEXPR_CHECK((xstd::unsigned_abs(T{+2}) == U{2}));
        XSTD_CONSTEXPR_CHECK((xstd::unsigned_abs(T{-2}) == U{2}));
        XSTD_CONSTEXPR_CHECK((xstd::unsigned_abs(limits::min()) == static_cast<U>(static_cast<U>(limits::max()) + U{1})));

        XSTD_CONSTEXPR_CHECK((xstd::sign(T{-2}) == -1));
        XSTD_CONSTEXPR_CHECK((xstd::sign(T{0}) == 0));
        XSTD_CONSTEXPR_CHECK((xstd::sign(T{+2}) == +1));

        XSTD_CONSTEXPR_CHECK((xstd::div(T{+8}, T{+3}) == xstd::div_t{T{+2}, T{+2}}));
        XSTD_CONSTEXPR_CHECK((xstd::div(T{-8}, T{+3}) == xstd::div_t{T{-2}, T{-2}}));
        XSTD_CONSTEXPR_CHECK((xstd::div(T{-8}, T{-3}) == xstd::div_t{T{+2}, T{-2}}));

        // A nonnegative remainder, then a negative one against each sign of
        // denom: all three arms of euclidean_div's adjustment, and both of
        // floored_div's.
        XSTD_CONSTEXPR_CHECK((xstd::euclidean_div(T{+8}, T{+3}) == xstd::div_t{T{+2}, T{+2}}));
        XSTD_CONSTEXPR_CHECK((xstd::euclidean_div(T{-8}, T{+3}) == xstd::div_t{T{-3}, T{+1}}));
        XSTD_CONSTEXPR_CHECK((xstd::euclidean_div(T{-8}, T{-3}) == xstd::div_t{T{+3}, T{+1}}));

        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{+8}, T{+3}) == xstd::div_t{T{+2}, T{+2}}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-8}, T{+3}) == xstd::div_t{T{-3}, T{+1}}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-8}, T{-3}) == xstd::div_t{T{+2}, T{-2}}));

        // denom == MIN, whose magnitude T cannot hold: in contract for all
        // three, and the reason the postconditions are written with unsigned_abs
        // rather than abs. The euclidean case is the one that would form -MIN
        // if the adjustment were spelled as a delta.
        XSTD_CONSTEXPR_CHECK((xstd::div(T{-1}, limits::min()) == xstd::div_t{T{0}, T{-1}}));
        XSTD_CONSTEXPR_CHECK((xstd::euclidean_div(T{-1}, limits::min()) == xstd::div_t{T{+1}, limits::max()}));
        XSTD_CONSTEXPR_CHECK((xstd::floored_div(T{-1}, limits::min()) == xstd::div_t{T{0}, T{-1}}));
}

// int8_t/../int64_t are aliases of the built-in types, but which alias maps
// to which built-in one is platform-dependent: on LP64 int64_t is long and
// long long is never instantiated above, on LLP64 it is the other way around.
// This battery closes that gap for whichever built-in width the exact-width
// list happens to miss. A plain function template rather than a second
// BOOST_AUTO_TEST_CASE_TEMPLATE, since the two lists overlap on every
// platform and duplicate registration of a test case name is an error.
//
template<std::signed_integral T>
auto check_built_in_width()
        -> void
{
        check_signed_integral_like<T>();
}

BOOST_AUTO_TEST_CASE(BuiltInWidths)
{
        check_built_in_width<int>();
        check_built_in_width<long>();
        check_built_in_width<long long>();
        check_built_in_width<std::intmax_t>();
}

// Platform details stay behind the xstd aliases; consumers and tests use only
// the portable public names.
BOOST_AUTO_TEST_CASE(Int128Aliases)
{
        static_assert(xstd::signed_integral_like<xstd::int128>);
        static_assert(std::same_as<decltype(xstd::unsigned_abs(xstd::int128{})), xstd::uint128>);
        static_assert(std::same_as<decltype(xstd::div(xstd::int128{1}, xstd::int128{1})), xstd::div_t<xstd::int128>>);

        check_signed_integral_like<xstd::int128>();
}

// clang-format off
auto const input = std::array<std::pair<int, int>, 8>
{{
        {+8, +3}, {+8, -3}, {-8, +3}, {-8, -3},
        {+1, +2}, {+1, -2}, {-1, +2}, {-1, -2}
}};
// clang-format on

BOOST_AUTO_TEST_CASE(StdDiv)
{
        // clang-format off
        auto const std_div = std::vector<xstd::div_t<int>>
        {
                {+2, +2}, {-2, +2}, {-2, -2}, {+2, -2},
                { 0, +1}, { 0, +1}, { 0, -1}, { 0, -1}
        };
        // clang-format on

        std::vector<xstd::div_t<int>> std_res;
        std::ranges::transform(input, std::back_inserter(std_res), [](auto const& p) -> xstd::div_t<int> {
                auto const d = std::div(p.first, p.second);
                return {.quot = d.quot, .rem = d.rem};
        });

        BOOST_CHECK(std_res == std_div);
}

BOOST_AUTO_TEST_SUITE_END()
