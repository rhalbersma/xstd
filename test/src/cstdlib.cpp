//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib.hpp>                // complete arithmetic surface
#include <xstd/cstdint.hpp>                // int128, uint128
#include <xstd/concepts.hpp>               // integral_like, signed_integral_like
#include <xstd/type_traits.hpp>            // make_unsigned_like_t
#include <xstd/test/absl_int128.hpp>       // XSTD_TEST_HAS_ABSL_INT128
#include <xstd/test/boost_int128.hpp>      // XSTD_TEST_HAS_BOOST_INT128
#include <xstd/test/exact_width_types.hpp> // std_signed_types
#include <xstd/test/constexpr.hpp>         // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp>        // Boost.Test
#include <algorithm>                       // ranges::transform
#include <array>                           // array
#include <concepts>                        // integral, same_as, signed_integral
#include <cstdint>                         // exact-width integer types, intmax_t
#include <cstdlib>                         // div
#include <iterator>                        // back_inserter
#include <limits>                          // numeric_limits
#include <type_traits>                     // make_unsigned_t
#include <utility>                         // pair
#include <vector>                          // vector

BOOST_AUTO_TEST_SUITE(CStdLib)

// Named concepts rather than bare requires-expressions: an invalid operand
// that is also non-dependent is a hard error on GCC.
template<class T>
concept has_abs = requires (T x) { xstd::abs(x); };

template<class T>
concept has_unsigned_abs = requires (T x) { xstd::unsigned_abs(x); };

template<class T>
concept has_sign = requires (T x) { xstd::sign(x); };

template<class T, class U>
concept has_div = requires (T numer, U denom) { xstd::div(numer, denom); };

template<class T>
concept has_euclidean_div = requires (T numer, T denom) { xstd::euclidean_div(numer, denom); };

template<class T>
concept has_floored_div = requires (T numer, T denom) { xstd::floored_div(numer, denom); };

// The constraint itself: integral-like in, nothing else, and the argument type
// comes back out rather than the promoted type. The standard's widths alone,
// std::make_unsigned_t below naming no other, and what one constrained
// template buys over abs/labs/llabs/imaxabs: int8_t and int16_t have no name
// in that scheme, and the two that do no longer need one.
BOOST_AUTO_TEST_CASE_TEMPLATE(Constraints, T, xstd::test::std_signed_types)
{
        static_assert(std::same_as<decltype(xstd::abs(T{})), T>);
        static_assert(std::same_as<decltype(xstd::unsigned_abs(T{})), std::make_unsigned_t<T>>);
        static_assert(std::same_as<decltype(xstd::sign(T{})), int>);
        static_assert(std::same_as<decltype(xstd::div(T{1}, T{1})), xstd::div_t<T>>);
        static_assert(std::same_as<decltype(xstd::euclidean_div(T{1}, T{1})), xstd::div_t<T>>);
        static_assert(std::same_as<decltype(xstd::floored_div(T{1}, T{1})), xstd::div_t<T>>);

        // Every type on these lists annotates its operations; one that does
        // not is covered by UnannotatedThirdPartyIntegerClassType below.
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

        // Unsigned arguments are inside it too, at every width, and answer in
        // their own type. abs and unsigned_abs coincide there, differing in
        // nothing a signature can show.
        using U = std::make_unsigned_t<T>;
        static_assert(has_abs<U>);
        static_assert(has_unsigned_abs<U>);
        static_assert(has_sign<U>);
        static_assert(has_div<U, U>);

        static_assert(std::same_as<decltype(xstd::abs(U{})), U>);
        static_assert(std::same_as<decltype(xstd::unsigned_abs(U{})), U>);
        static_assert(std::same_as<decltype(xstd::sign(U{})), int>);
        static_assert(std::same_as<decltype(xstd::div(U{1}, U{1})), xstd::div_t<U>>);
        static_assert(std::same_as<decltype(xstd::euclidean_div(U{1}, U{1})), xstd::div_t<U>>);
        static_assert(std::same_as<decltype(xstd::floored_div(U{1}, U{1})), xstd::div_t<U>>);

        static_assert(noexcept(xstd::abs(U{})));
        static_assert(noexcept(xstd::unsigned_abs(U{})));
        static_assert(noexcept(xstd::sign(U{})));
        static_assert(noexcept(xstd::div(U{1}, U{1})));
        static_assert(noexcept(xstd::euclidean_div(U{1}, U{1})));
        static_assert(noexcept(xstd::floored_div(U{1}, U{1})));

        BOOST_CHECK(true); // silence Boost.Test's "test case did not check any assertions"
}

BOOST_AUTO_TEST_CASE(NonIntegralLikeArgumentsAreRejected)
{
        static_assert(not has_abs<double>);
        static_assert(not has_sign<char*>);

        // Answers rather than hard errors only because the header deduces
        // unsigned_abs's return type; Clang before 21 has no CWG2369.
        static_assert(not has_unsigned_abs<double>);
        static_assert(not has_unsigned_abs<char*>);

        // bool satisfies integral_like - it is unsigned-like, having a
        // numeric_limits that says integer and not signed - so it reaches each
        // of these on the constraint alone. Every one is deleted instead, as
        // xstd::to_chars is and for its reason. Four of the six have to be:
        // unsigned_abs forms make_unsigned_like_t<I> in its body, where
        // make_unsigned_like<bool> is the empty primary and the failure is no
        // longer in the immediate context, so without the deletion the call is
        // ill-formed rather than unsatisfied - and div reaches that through its
        // own postconditions, the two named divisions through div. sign and abs
        // would merely have answered 1 and true.
        static_assert(not has_abs<bool>);
        static_assert(not has_sign<bool>);
        static_assert(not has_unsigned_abs<bool>);
        static_assert(not has_div<bool, bool>);
        static_assert(not has_euclidean_div<bool>);
        static_assert(not has_floored_div<bool>);

        // Both parameters deduce the same T, so a mixed-width call is a
        // deduction failure rather than a silent conversion of one operand.
        static_assert(not has_div<std::int32_t, std::int64_t>);
        static_assert(std::same_as<decltype(xstd::div<std::int64_t>(8, 3)), xstd::div_t<std::int64_t>>);

        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ExactDivisions, T, xstd::test::std_signed_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{+6}, T{+3})), (xstd::div_t<T>{+2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{+6}, T{+3})), (xstd::div_t<T>{+2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{+6}, T{+3})), (xstd::div_t<T>{+2, 0}));

        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{+6}, T{-3})), (xstd::div_t<T>{-2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{+6}, T{-3})), (xstd::div_t<T>{-2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{+6}, T{-3})), (xstd::div_t<T>{-2, 0}));

        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{-6}, T{+3})), (xstd::div_t<T>{-2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{-6}, T{+3})), (xstd::div_t<T>{-2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{-6}, T{+3})), (xstd::div_t<T>{-2, 0}));

        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{-6}, T{-3})), (xstd::div_t<T>{+2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{-6}, T{-3})), (xstd::div_t<T>{+2, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{-6}, T{-3})), (xstd::div_t<T>{+2, 0}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BoundaryDivisions, T, xstd::test::std_signed_types)
{
        using limits = std::numeric_limits<T>;
        constexpr auto min = limits::min();
        constexpr auto max = limits::max();

        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(min, T{+1})), (xstd::div_t<T>{min, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(min, T{+1})), (xstd::div_t<T>{min, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(min, T{+1})), (xstd::div_t<T>{min, 0}));

        // In contract though |MIN| is not representable, which is why the
        // postconditions are written with unsigned_abs rather than abs.
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{+1}, min)), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{+1}, min)), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{+1}, min)), (xstd::div_t<T>{-1, static_cast<T>(min + 1)}));

        // The same with a negative remainder, selecting euclidean_div's
        // negative adjustment, which spelled as a delta would form -MIN.
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{-1}, min)), (xstd::div_t<T>{0, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{-1}, min)), (xstd::div_t<T>{+1, max}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{-1}, min)), (xstd::div_t<T>{0, -1}));

        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(max, T{-1})), (xstd::div_t<T>{static_cast<T>(-max), 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(max, T{-1})), (xstd::div_t<T>{static_cast<T>(-max), 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(max, T{-1})), (xstd::div_t<T>{static_cast<T>(-max), 0}));

        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(min, max)), (xstd::div_t<T>{-1, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(min, max)), (xstd::div_t<T>{-2, static_cast<T>(max - 1)}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(min, max)), (xstd::div_t<T>{-2, static_cast<T>(max - 1)}));
}

// The whole surface at one type, both arms of every conditional, on a type
// std::signed_integral can never accept.
template<xstd::signed_integral_like S>
auto check_signed_integral_like()
        -> void
{
        using U = xstd::make_unsigned_like_t<S>;
        using limits = std::numeric_limits<S>;

        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::abs(S{-2})), (S{2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::abs(S{+2})), (S{2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::abs(static_cast<S>(limits::min() + S{1}))), (limits::max()));

        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::unsigned_abs(S{+2})), (U{2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::unsigned_abs(S{-2})), (U{2}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::unsigned_abs(limits::min())), (static_cast<U>(static_cast<U>(limits::max()) + U{1})));

        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::sign(S{-2})), (-1));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::sign(S{0})), (0));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::sign(S{+2})), (+1));

        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(S{+8}, S{+3})), (xstd::div_t{S{+2}, S{+2}}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(S{-8}, S{+3})), (xstd::div_t{S{-2}, S{-2}}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(S{-8}, S{-3})), (xstd::div_t{S{+2}, S{-2}}));

        // All three arms of euclidean_div's adjustment and both of
        // floored_div's.
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(S{+8}, S{+3})), (xstd::div_t{S{+2}, S{+2}}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(S{-8}, S{+3})), (xstd::div_t{S{-3}, S{+1}}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(S{-8}, S{-3})), (xstd::div_t{S{+3}, S{+1}}));

        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(S{+8}, S{+3})), (xstd::div_t{S{+2}, S{+2}}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(S{-8}, S{+3})), (xstd::div_t{S{-3}, S{+1}}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(S{-8}, S{-3})), (xstd::div_t{S{+2}, S{-2}}));

        // denom == MIN, whose magnitude S cannot hold: in contract for all
        // three, and what the unsigned_abs postconditions are written for.
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(S{-1}, limits::min())), (xstd::div_t{S{0}, S{-1}}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(S{-1}, limits::min())), (xstd::div_t{S{+1}, limits::max()}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(S{-1}, limits::min())), (xstd::div_t{S{0}, S{-1}}));
}

// Which exact-width alias maps to which built-in is platform-dependent, so one
// built-in width is never instantiated above; this closes that gap.
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

// The same battery over a 128-bit type the library does not know about, which
// xstd::int128 cannot show: it arrives along a path no third party's type has.
#ifdef XSTD_TEST_HAS_BOOST_INT128

BOOST_AUTO_TEST_CASE(ThirdPartyIntegerClassType)
{
        using T = xstd::test::boost_int128;

        static_assert(xstd::signed_integral_like<T>);
        static_assert(std::same_as<decltype(xstd::unsigned_abs(T{})), xstd::test::boost_uint128>);
        static_assert(std::same_as<decltype(xstd::div(T{1}, T{1})), xstd::div_t<T>>);

        check_signed_integral_like<T>();
}

#endif

// The other arm of the conditional exception specification, from a third
// party's type as well: absl::int128 declares not one noexcept in its header,
// so neither do the six functions when called with it.
#ifdef XSTD_TEST_HAS_ABSL_INT128

BOOST_AUTO_TEST_CASE(UnannotatedThirdPartyIntegerClassType)
{
        using T = xstd::test::absl_int128;

        static_assert(xstd::integral_like<T>);
        static_assert(xstd::signed_integral_like<T>);
        static_assert(std::same_as<decltype(xstd::unsigned_abs(T{})), xstd::test::absl_uint128>);
        static_assert(std::same_as<decltype(xstd::div(T{1}, T{1})), xstd::div_t<T>>);

        static_assert(not noexcept(xstd::abs(T{1})));
        static_assert(not noexcept(xstd::unsigned_abs(T{1})));
        static_assert(not noexcept(xstd::sign(T{1})));
        static_assert(not noexcept(xstd::div(T{1}, T{1})));
        static_assert(not noexcept(xstd::euclidean_div(T{1}, T{1})));
        static_assert(not noexcept(xstd::floored_div(T{1}, T{1})));

        // A built-in still is, which an unconditional removal would have lost.
        static_assert(noexcept(xstd::abs(1)));
        static_assert(noexcept(xstd::sign(1)));
        static_assert(noexcept(xstd::div(1, 1)));

#ifdef ABSL_HAVE_INTRINSIC_INT128
        check_signed_integral_like<T>();
#else
        // Without a 128-bit intrinsic to lower them to, its operator/ and
        // operator% are out-of-line in int128.cc and not constexpr, so the
        // battery above cannot be constant-evaluated there. The values still
        // have to come out, which is what is left to check.
        BOOST_CHECK(xstd::abs(T{-2}) == T{2});
        BOOST_CHECK(xstd::unsigned_abs(T{-2}) == xstd::test::absl_uint128{2});
        BOOST_CHECK(xstd::sign(T{-2}) == -1);
        BOOST_CHECK((xstd::div(T{-8}, T{+3}) == xstd::div_t{T{-2}, T{-2}}));
        BOOST_CHECK((xstd::euclidean_div(T{-8}, T{+3}) == xstd::div_t{T{-3}, T{+1}}));
        BOOST_CHECK((xstd::floored_div(T{-8}, T{+3}) == xstd::div_t{T{-3}, T{+1}}));
#endif
}

#endif

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
