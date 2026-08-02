//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib.hpp>            // abs, uabs, sign, div_t, div, euclidean_div, floored_div
#include <xstd/concepts.hpp>           // signed_integral_like
#include <xstd/type_traits.hpp>        // make_unsigned_like_t
#include <xstd/test/constexpr.hpp>     // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <xstd/test/integer_class.hpp> // signed_integer_class, unsigned_integer_class
#include <boost/test/unit_test.hpp>    // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS
#include <algorithm>                   // ranges::transform
#include <array>                       // array
#include <concepts>                    // integral, same_as, signed_integral
#include <cstdint>                     // int8_t, int16_t, int32_t, int64_t, intmax_t
#include <cstdlib>                     // div
#include <format>                      // format
#include <iterator>                    // back_inserter
#include <limits>                      // numeric_limits
#include <sstream>                     // ostringstream
#include <tuple>                       // tuple
#include <type_traits>                 // make_unsigned_t
#include <utility>                     // pair
#include <vector>                      // vector

BOOST_AUTO_TEST_SUITE(CStdLib)

// The exact-width signed types, which is what one constrained template buys
// over the <cstdlib>-style abs/labs/llabs/imaxabs naming: int8_t and int16_t
// have no name in that scheme at all, and the two that do (int32_t, int64_t)
// no longer need one. The two shapes of 128-bit integer - the built-in
// __int128 and a class type - get their own cases further down, since neither
// can appear in a list of built-in widths.
using exact_width_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;

// Named concepts rather than bare requires-expressions in the test bodies:
// a requires-expression whose operand is invalid *and* non-dependent is a
// hard error on GCC, so the type has to stay a template parameter.
template<class T>
concept has_abs = requires (T x) { xstd::abs(x); };

template<class T>
concept has_uabs = requires (T x) { xstd::uabs(x); };

template<class T>
concept has_sign = requires (T x) { xstd::sign(x); };

template<class T, class U>
concept has_div = requires (T numer, U denom) { xstd::div(numer, denom); };

// The constraint itself: signed integral in, nothing else, and the argument
// type comes back out rather than the promoted type.
BOOST_AUTO_TEST_CASE_TEMPLATE(Constraints, T, exact_width_types)
{
        static_assert(std::same_as<decltype(xstd::abs(T{})), T>);
        static_assert(std::same_as<decltype(xstd::uabs(T{})), std::make_unsigned_t<T>>);
        static_assert(std::same_as<decltype(xstd::sign(T{})), int>);
        static_assert(std::same_as<decltype(xstd::div(T{1}, T{1})), xstd::div_t<T>>);
        static_assert(std::same_as<decltype(xstd::euclidean_div(T{1}, T{1})), xstd::div_t<T>>);
        static_assert(std::same_as<decltype(xstd::floored_div(T{1}, T{1})), xstd::div_t<T>>);

        // the noexcept specification is conditional, and the condition holds
        // at every built-in width - it withdraws only for a type whose own
        // arithmetic can throw, which no built-in one can
        static_assert(xstd::is_nothrow_signed_integral_like_v<T>);
        static_assert(noexcept(xstd::abs(T{})));
        static_assert(noexcept(xstd::uabs(T{})));
        static_assert(noexcept(xstd::sign(T{})));
        static_assert(noexcept(xstd::div(T{1}, T{1})));
        static_assert(noexcept(xstd::euclidean_div(T{1}, T{1})));
        static_assert(noexcept(xstd::floored_div(T{1}, T{1})));

        static_assert(has_abs<T>);
        static_assert(has_uabs<T>);
        static_assert(has_sign<T>);
        static_assert(has_div<T, T>);

        // Unsigned arguments are outside the constraint, at every width.
        using U = std::make_unsigned_t<T>;
        static_assert(not has_abs<U>);
        static_assert(not has_uabs<U>);
        static_assert(not has_sign<U>);
        static_assert(not has_div<U, U>);

        BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
}

BOOST_AUTO_TEST_CASE(NonSignedIntegralArgumentsAreRejected)
{
        static_assert(not has_abs<bool>);
        static_assert(not has_abs<double>);
        static_assert(not has_sign<char*>);

        // uabs is the only one of the three whose return type is computed by a
        // trait, so it is the only one where rejecting a non-integral argument
        // depends on the trait never being instantiated. Clang before 21 does
        // not implement CWG2369 and substitutes the return type before checking
        // the constraint, so spelling std::make_unsigned_t<T> in the signature
        // makes the double and char* cases a hard error rather than a
        // substitution failure - these two stop compiling instead of being
        // false. The deduced return type in the header is what keeps them
        // compiling; see doc/design.md.
        static_assert(not has_uabs<bool>);
        static_assert(not has_uabs<double>);
        static_assert(not has_uabs<char*>);

        // Both parameters deduce the same T, so a mixed-width call is a
        // deduction failure rather than a silent conversion of one operand.
        static_assert(not has_div<std::int32_t, std::int64_t>);
        static_assert(std::same_as<decltype(xstd::div<std::int64_t>(8, 3)), xstd::div_t<std::int64_t>>);

        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Abs, T, exact_width_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{-2}), T{2});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{-1}), T{1});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{0}), T{0});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{+1}), T{1});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{+2}), T{2});

        using limits = std::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(limits::max()), limits::max());
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(static_cast<T>(limits::min() + 1)), limits::max());
}

// The div families exercise uabs transitively through their assert() guards;
// check the MIN-boundary wraparound directly and at compile time, since that
// is both what distinguishes uabs from abs and the one case a widening-based
// |x| could not have handled - least of all at the widest width, which has
// nothing to widen to.
BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedAbs, T, exact_width_types)
{
        using U = std::make_unsigned_t<T>;

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::uabs(T{-2}), U{2});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::uabs(T{0}), U{0});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::uabs(T{+2}), U{2});

        using limits = std::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::uabs(limits::min()), static_cast<U>(static_cast<U>(limits::max()) + U{1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::uabs(limits::max()), static_cast<U>(limits::max()));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Sign, T, exact_width_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{-2}), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{-1}), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{0}), 0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{+1}), +1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{+2}), +1);

        using limits = std::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(limits::min()), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(limits::max()), +1);
}

// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

BOOST_AUTO_TEST_CASE_TEMPLATE(TruncatedDiv, T, exact_width_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{+8}, T{+3}), (xstd::div_t<T>{+2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{+8}, T{-3}), (xstd::div_t<T>{-2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{-8}, T{+3}), (xstd::div_t<T>{-2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{-8}, T{-3}), (xstd::div_t<T>{+2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{+1}, T{+2}), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{+1}, T{-2}), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{-1}, T{+2}), (xstd::div_t<T>{0, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{-1}, T{-2}), (xstd::div_t<T>{0, -1}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(EuclideanDiv, T, exact_width_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{+8}, T{+3}), (xstd::div_t<T>{+2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{+8}, T{-3}), (xstd::div_t<T>{-2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{-8}, T{+3}), (xstd::div_t<T>{-3, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{-8}, T{-3}), (xstd::div_t<T>{+3, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{+1}, T{+2}), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{+1}, T{-2}), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{-1}, T{+2}), (xstd::div_t<T>{-1, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{-1}, T{-2}), (xstd::div_t<T>{+1, +1}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(FlooredDiv, T, exact_width_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{+8}, T{+3}), (xstd::div_t<T>{+2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{+8}, T{-3}), (xstd::div_t<T>{-3, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{-8}, T{+3}), (xstd::div_t<T>{-3, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{-8}, T{-3}), (xstd::div_t<T>{+2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{+1}, T{+2}), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{+1}, T{-2}), (xstd::div_t<T>{-1, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{-1}, T{+2}), (xstd::div_t<T>{-1, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{-1}, T{-2}), (xstd::div_t<T>{0, -1}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ExactDivisions, T, exact_width_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{+6}, T{+3}), (xstd::div_t<T>{+2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{+6}, T{+3}), (xstd::div_t<T>{+2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{+6}, T{+3}), (xstd::div_t<T>{+2, 0}));

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{+6}, T{-3}), (xstd::div_t<T>{-2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{+6}, T{-3}), (xstd::div_t<T>{-2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{+6}, T{-3}), (xstd::div_t<T>{-2, 0}));

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{-6}, T{+3}), (xstd::div_t<T>{-2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{-6}, T{+3}), (xstd::div_t<T>{-2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{-6}, T{+3}), (xstd::div_t<T>{-2, 0}));

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{-6}, T{-3}), (xstd::div_t<T>{+2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{-6}, T{-3}), (xstd::div_t<T>{+2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{-6}, T{-3}), (xstd::div_t<T>{+2, 0}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BoundaryDivisions, T, exact_width_types)
{
        using limits = std::numeric_limits<T>;
        constexpr auto min = limits::min();
        constexpr auto max = limits::max();

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(min, T{+1}), (xstd::div_t<T>{min, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(min, T{+1}), (xstd::div_t<T>{min, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(min, T{+1}), (xstd::div_t<T>{min, 0}));

        // denom == MIN is in contract even though |MIN| is not representable
        // in T: it is exactly why the postconditions are written with uabs
        // rather than abs.
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{+1}, min), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{+1}, min), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{+1}, min), (xstd::div_t<T>{-1, static_cast<T>(min + 1)}));

        // denom == MIN again, but with a negative remainder, which is what
        // selects euclidean_div's negative adjustment. Computing that one as
        // rem + I * denom would form -MIN and overflow; being a constant
        // expression, this check catches that at compile time as well as
        // under a sanitizer. Promotion hides it below int, so the wider
        // instantiations of this case are the ones that matter.
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(T{-1}, min), (xstd::div_t<T>{0, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(T{-1}, min), (xstd::div_t<T>{+1, max}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(T{-1}, min), (xstd::div_t<T>{0, -1}));

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(max, T{-1}), (xstd::div_t<T>{static_cast<T>(-max), 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(max, T{-1}), (xstd::div_t<T>{static_cast<T>(-max), 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(max, T{-1}), (xstd::div_t<T>{static_cast<T>(-max), 0}));

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::div(min, max), (xstd::div_t<T>{-1, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::euclidean_div(min, max), (xstd::div_t<T>{-2, static_cast<T>(max - 1)}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::floored_div(min, max), (xstd::div_t<T>{-2, static_cast<T>(max - 1)}));
}

// The formatting counterpart of XSTD_CONSTEXPR_CHECK_EQUAL, which cannot be
// used directly here: std::format is not a constant expression before P3391
// (constexpr std::format, plenary-approved for C++29), so the static_assert
// half only exists once a standard library announces the feature. Checking
// the formatted value at compile time is what pins xstd::div_t's own
// conditional constexpr in <xstd/cstdlib.hpp> to something observable -
// without it, the macro there could quietly expand to nothing forever.
#ifdef __cpp_lib_constexpr_format
#define XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL(a, b) XSTD_CONSTEXPR_CHECK_EQUAL((a), (b))
#else
#define XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL(a, b) BOOST_CHECK_EQUAL((a), (b))
#endif

BOOST_AUTO_TEST_CASE_TEMPLATE(Formatter, T, exact_width_types)
{
        XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL(std::format("{}", xstd::div_t<T>{1, -2}), "(1, -2)");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(StreamInsertion, T, exact_width_types)
{
        std::ostringstream oss;
        oss << xstd::div_t<T>{1, -2};
        BOOST_CHECK_EQUAL(oss.str(), "(1, -2)");
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

        XSTD_CONSTEXPR_CHECK((xstd::uabs(T{+2}) == U{2}));
        XSTD_CONSTEXPR_CHECK((xstd::uabs(T{-2}) == U{2}));
        XSTD_CONSTEXPR_CHECK((xstd::uabs(limits::min()) == static_cast<U>(static_cast<U>(limits::max()) + U{1})));

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
        // three, and the reason the postconditions are written with uabs
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
// The formatting half stays here rather than moving into the battery above:
// it is what leaves div_t's formatter and operator<< executed rather than
// merely instantiated - Boost.Test's printing machinery instantiates them for
// anything it might have to report, but only runs them when an assertion
// fails - and it is exactly the part an integer-class type need not support.
template<std::signed_integral T>
auto check_built_in_width()
        -> void
{
        check_signed_integral_like<T>();

        XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL(std::format("{}", xstd::div_t<T>{1, -2}), "(1, -2)");

        std::ostringstream oss;
        oss << xstd::div_t<T>{1, -2};
        BOOST_CHECK_EQUAL(oss.str(), "(1, -2)");
}

BOOST_AUTO_TEST_CASE(BuiltInWidths)
{
        check_built_in_width<int>();
        check_built_in_width<long>();
        check_built_in_width<long long>();
        check_built_in_width<std::intmax_t>();
}

// The two shapes a 128-bit integer comes in, and the reason the constraint is
// xstd::signed_integral_like rather than std::signed_integral.
//
// __int128 is a built-in type whose std::is_integral answer is a property of
// the dialect rather than of the type: libstdc++ withholds it outside GNU
// mode - the strictly conforming mode this project builds in - while libc++
// gives it in every mode. std::signed_integral therefore covers this type on
// some standard libraries and not others, at no fault of the type's.
// Naming it is what -Wpedantic is for, hence the same suppression the header
// carries.
#ifdef __SIZEOF_INT128__
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

// __SIZEOF_INT128__ says the *compiler* has the type. It says nothing about
// whether the *standard library* describes it, and those come apart: clang-cl
// has __int128 on x64, but the MSVC STL specializes neither std::is_integral
// nor std::numeric_limits for it, so nothing there says it is an integer and
// integral_like correctly declines. numeric_limits is the gate rather than a
// standard-library predefine because it is the thing integral_like actually
// reads - if a library ever specializes it, this case starts testing on its
// own, and if one specializes it while something else is missing, the
// assertions below still fire rather than being skipped.
//
// A template so that the discarded branch is never instantiated: in a
// non-templated context a discarded if-constexpr statement is still fully
// checked, static_asserts included.
template<class T>
auto check_int128()
        -> void
{
        if constexpr (std::numeric_limits<T>::is_specialized) {
                static_assert(xstd::signed_integral_like<T>);
                static_assert(std::same_as<decltype(xstd::uabs(T{})), xstd::make_unsigned_like_t<T>>);
                static_assert(std::same_as<decltype(xstd::div(T{1}, T{1})), xstd::div_t<T>>);

                check_signed_integral_like<T>();
        }
}

BOOST_AUTO_TEST_CASE(Int128)
{
        check_int128<__int128>();

        BOOST_CHECK(true); // the MSVC STL leaves __int128 undescribed, see above
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif

// A class type is the shape no dialect could ever make integral, and the one
// the standard library's own 128-bit types take where there is no built-in
// one to reach for (libstdc++'s std::ranges::__detail::__max_diff_type, the
// MSVC STL's std::_Signed128). It qualifies by behaving like an integer:
// see <xstd/test/integer_class.hpp>.
BOOST_AUTO_TEST_CASE(IntegerClassType)
{
        using T = xstd::test::signed_integer_class;

        static_assert(not std::integral<T>);
        static_assert(xstd::signed_integral_like<T>);
        static_assert(std::same_as<decltype(xstd::uabs(T{})), xstd::test::unsigned_integer_class>);
        static_assert(std::same_as<decltype(xstd::abs(T{})), T>);
        static_assert(std::same_as<decltype(xstd::sign(T{})), int>);
        static_assert(std::same_as<decltype(xstd::div(T{1}, T{1})), xstd::div_t<T>>);

        // an unsigned integer-like type is still rejected, at a class type
        // just as at a built-in one
        static_assert(not has_abs<xstd::test::unsigned_integer_class>);
        static_assert(not has_uabs<xstd::test::unsigned_integer_class>);
        static_assert(not has_div<xstd::test::unsigned_integer_class, xstd::test::unsigned_integer_class>);

        check_signed_integral_like<T>();
}

// Class template argument deduction: div_t{q, r} still spells the result of
// a call to div at the argument's own width, the way the four separate
// div_t/ldiv_t/lldiv_t/imaxdiv_t names used to.
BOOST_AUTO_TEST_CASE(DeducedDivT)
{
        static_assert(std::same_as<decltype(xstd::div_t{1, 2}), xstd::div_t<int>>);
        static_assert(std::same_as<decltype(xstd::div_t{1L, 2L}), xstd::div_t<long>>);
        static_assert(std::same_as<decltype(xstd::div_t{std::int8_t{1}, std::int8_t{2}}), xstd::div_t<std::int8_t>>);

        BOOST_CHECK(true);
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
                return {d.quot, d.rem};
        });

        BOOST_CHECK_EQUAL_COLLECTIONS(
                std_res.begin(), std_res.end(),
                std_div.begin(), std_div.end());
}

BOOST_AUTO_TEST_SUITE_END()
