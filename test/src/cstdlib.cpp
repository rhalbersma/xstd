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
#include <xstd/test/integer_class.hpp>     // conforming_int_class, unregistered_int_class
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

// Named concepts, not bare requires-expressions: a non-dependent invalid operand hard-errors.
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

// The constraint itself: integral-like in, and the argument type back out, not a promotion.
BOOST_AUTO_TEST_CASE_TEMPLATE(Constraints, T, xstd::test::std_signed_types)
{
        static_assert(std::same_as<decltype(xstd::abs(T{})), T>);
        static_assert(std::same_as<decltype(xstd::unsigned_abs(T{})), std::make_unsigned_t<T>>);
        static_assert(std::same_as<decltype(xstd::sign(T{})), int>);
        static_assert(std::same_as<decltype(xstd::div(T{1}, T{1})), xstd::div_t<T>>);
        static_assert(std::same_as<decltype(xstd::euclidean_div(T{1}, T{1})), xstd::div_t<T>>);
        static_assert(std::same_as<decltype(xstd::floored_div(T{1}, T{1})), xstd::div_t<T>>);

        // Every type here annotates its operations; one that does not is covered below.
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

        // Unsigned is inside it too, at every width; abs and unsigned_abs coincide there.
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

        // An answer, not a hard error, only because unsigned_abs deduces its return type.
        static_assert(not has_unsigned_abs<double>);
        static_assert(not has_unsigned_abs<char*>);

        // bool is unsigned-like, so the constraint admits it; all six delete it instead.
        static_assert(not has_abs<bool>);
        static_assert(not has_sign<bool>);
        static_assert(not has_unsigned_abs<bool>);
        static_assert(not has_div<bool, bool>);
        static_assert(not has_euclidean_div<bool>);
        static_assert(not has_floored_div<bool>);

        // Both parameters deduce one T, so a mixed-width call fails rather than converting.
        static_assert(not has_div<std::int32_t, std::int64_t>);
        static_assert(std::same_as<decltype(xstd::div<std::int64_t>(8, 3)), xstd::div_t<std::int64_t>>);

        BOOST_CHECK(true);
}

// An integer-class type the library has not been told the unsigned counterpart of. Every
// other fixture in the suite arrives with that association already written, so this is the
// only one that reaches the constraint. It matters that these are answers: the functions
// form make_unsigned_like_t in their bodies, and div reaches it from inside an assert, so
// left to the body this type would compile under NDEBUG and fail to compile without it.
BOOST_AUTO_TEST_CASE(UnregisteredIntegerClassTypeIsOutsideTheDomain)
{
        using T = xstd::test::unregistered_int_class;

        static_assert(xstd::integral_like<T>);
        static_assert(xstd::signed_integral_like<T>);
        static_assert(not xstd::has_unsigned_counterpart<T>);

        static_assert(not has_unsigned_abs<T>);
        static_assert(not has_div<T, T>);
        static_assert(not has_euclidean_div<T>);
        static_assert(not has_floored_div<T>);

        // abs and sign never form the counterpart, so they take it as they take any width.
        static_assert(has_abs<T>);
        static_assert(has_sign<T>);

        // And the conforming unsigned fixture is inside the domain, being its own counterpart.
        using U = xstd::test::conforming_int_class;

        static_assert(xstd::has_unsigned_counterpart<U>);
        static_assert(has_unsigned_abs<U> and has_div<U, U>);
        static_assert(has_euclidean_div<U> and has_floored_div<U>);

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

        // In contract though |MIN| is not representable: why the postconditions use unsigned_abs.
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(T{+1}, min)), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(T{+1}, min)), (xstd::div_t<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(T{+1}, min)), (xstd::div_t<T>{-1, static_cast<T>(min + 1)}));

        // The same with a negative remainder, which spelled as a delta would form -MIN.
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

// The whole surface at one type std::signed_integral can never accept, both arms of each.
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

        // All three arms of euclidean_div's adjustment and both of floored_div's.
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(S{+8}, S{+3})), (xstd::div_t{S{+2}, S{+2}}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(S{-8}, S{+3})), (xstd::div_t{S{-3}, S{+1}}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(S{-8}, S{-3})), (xstd::div_t{S{+3}, S{+1}}));

        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(S{+8}, S{+3})), (xstd::div_t{S{+2}, S{+2}}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(S{-8}, S{+3})), (xstd::div_t{S{-3}, S{+1}}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(S{-8}, S{-3})), (xstd::div_t{S{+2}, S{-2}}));

        // denom == MIN, whose magnitude S cannot hold: what unsigned_abs is there for.
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::div(S{-1}, limits::min())), (xstd::div_t{S{0}, S{-1}}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::euclidean_div(S{-1}, limits::min())), (xstd::div_t{S{+1}, limits::max()}));
        XSTD_CONSTEXPR_CHECK_EQUAL((xstd::floored_div(S{-1}, limits::min())), (xstd::div_t{S{0}, S{-1}}));
}

// Which exact-width alias maps to which built-in is platform-dependent; this closes the gap.
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

// Platform details stay behind the xstd aliases; only the public names are used.
BOOST_AUTO_TEST_CASE(Int128Aliases)
{
        static_assert(xstd::signed_integral_like<xstd::int128>);
        static_assert(std::same_as<decltype(xstd::unsigned_abs(xstd::int128{})), xstd::uint128>);
        static_assert(std::same_as<decltype(xstd::div(xstd::int128{1}, xstd::int128{1})), xstd::div_t<xstd::int128>>);

        check_signed_integral_like<xstd::int128>();
}

// The same battery over a type the library does not know about, which xstd::int128 cannot show.
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

// The other arm of the conditional noexcept: absl::int128 declares not one in its header.
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
        // Without an intrinsic its operator/ and operator% are not constexpr, so check at run time.
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
