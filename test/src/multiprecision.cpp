//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/cstdlib.hpp>                 // abs, uabs, sign, div_t, div, euclidean_div, floored_div
#include <xstd/concepts.hpp>                // signed_integral_like, unsigned_integral_like
#include <xstd/type_traits.hpp>             // make_unsigned_like, make_unsigned_like_t
#include <boost/multiprecision/cpp_int.hpp> // int128_t, int256_t, int512_t, int1024_t, uint128_t, uint256_t, uint512_t, uint1024_t
#include <boost/test/unit_test.hpp>         // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_CHECK
#include <concepts>                         // integral, same_as
#include <limits>                           // numeric_limits
#include <tuple>                            // tuple

// The concept's external validation: an integer-class type that xstd did not
// write, at the four widths Boost.Multiprecision's own documentation uses.
//
// <xstd/test/integer_class.hpp> models the category, but it was written
// against this concept and could have been fitted to it without anyone
// noticing. Boost.Multiprecision was not, so these checks answer a question
// the purpose-built fixture cannot: does a real integer-class type satisfy
// xstd::signed_integral_like as it stands, without being adapted to it?
//
// The answer is that it does, for the price of one specialization per type -
// the same one line a user writes for any integer type the compiler does not
// know about. Everything else these types already have.
//
// They also cover a representation nothing else in the suite does. cpp_int's
// signed types are *signed-magnitude*, so their range is symmetric:
// min() == -max(), and |min()| is representable where a two's complement type
// would need one more bit. Every other type tested here - the built-in widths,
// __int128, and the fixture - is two's complement with an asymmetric range, so
// this is what would catch a wraparound assumption baked into uabs.
namespace mp = boost::multiprecision;

template<>
struct xstd::make_unsigned_like<mp::int128_t>
{
        using type = mp::uint128_t;
};

template<>
struct xstd::make_unsigned_like<mp::int256_t>
{
        using type = mp::uint256_t;
};

template<>
struct xstd::make_unsigned_like<mp::int512_t>
{
        using type = mp::uint512_t;
};

template<>
struct xstd::make_unsigned_like<mp::int1024_t>
{
        using type = mp::uint1024_t;
};

BOOST_AUTO_TEST_SUITE(Multiprecision)

using cpp_int_widths = std::tuple<mp::int128_t, mp::int256_t, mp::int512_t, mp::int1024_t>;

// Deliberately BOOST_CHECK rather than the XSTD_CONSTEXPR_CHECK that
// test/src/cstdlib.cpp uses for every other type: cpp_int's arithmetic is not
// usable in a constant expression, so the static_assert half of that macro
// cannot be formed at all. The expected values mirror the battery there, which
// remains the source of truth for them; they are repeated rather than shared
// because the sharing would have to be through a macro that is half
// static_assert.
BOOST_AUTO_TEST_CASE_TEMPLATE(SignedIntegralLike, T, cpp_int_widths)
{
        using U = xstd::make_unsigned_like_t<T>;
        using limits = std::numeric_limits<T>;

        // no class type is ever std::integral, at any width
        static_assert(not std::integral<T>);
        static_assert(xstd::signed_integral_like<T>);
        static_assert(xstd::unsigned_integral_like<U>);
        static_assert(std::same_as<decltype(xstd::uabs(T{})), U>);
        static_assert(std::same_as<decltype(xstd::sign(T{})), int>);

        // cpp_int's operator/ throws std::overflow_error on a zero divisor,
        // so the division family withdraws its noexcept for exactly these
        // types. That is the whole point of the specification being
        // conditional rather than unconditional; see doc/design.md.
        static_assert(not xstd::is_nothrow_integral_like_v<T>);
        static_assert(not xstd::is_nothrow_signed_integral_like_v<T>);
        static_assert(not noexcept(xstd::div(T{1}, T{1})));
        static_assert(not noexcept(xstd::euclidean_div(T{1}, T{1})));
        static_assert(not noexcept(xstd::floored_div(T{1}, T{1})));
        static_assert(not noexcept(xstd::uabs(T{1})));

        BOOST_CHECK((xstd::abs(T{-2}) == T{2}));
        BOOST_CHECK((xstd::abs(T{+2}) == T{2}));

        BOOST_CHECK((xstd::uabs(T{-2}) == U{2}));
        BOOST_CHECK((xstd::uabs(T{+2}) == U{2}));

        BOOST_CHECK((xstd::sign(T{-2}) == -1));
        BOOST_CHECK((xstd::sign(T{0}) == 0));
        BOOST_CHECK((xstd::sign(T{+2}) == +1));

        BOOST_CHECK((xstd::div(T{+8}, T{+3}) == xstd::div_t<T>{T{+2}, T{+2}}));
        BOOST_CHECK((xstd::div(T{-8}, T{+3}) == xstd::div_t<T>{T{-2}, T{-2}}));
        BOOST_CHECK((xstd::div(T{-8}, T{-3}) == xstd::div_t<T>{T{+2}, T{-2}}));

        // all three arms of euclidean_div's adjustment, and both of
        // floored_div's
        BOOST_CHECK((xstd::euclidean_div(T{+8}, T{+3}) == xstd::div_t<T>{T{+2}, T{+2}}));
        BOOST_CHECK((xstd::euclidean_div(T{-8}, T{+3}) == xstd::div_t<T>{T{-3}, T{+1}}));
        BOOST_CHECK((xstd::euclidean_div(T{-8}, T{-3}) == xstd::div_t<T>{T{+3}, T{+1}}));

        BOOST_CHECK((xstd::floored_div(T{+8}, T{+3}) == xstd::div_t<T>{T{+2}, T{+2}}));
        BOOST_CHECK((xstd::floored_div(T{-8}, T{+3}) == xstd::div_t<T>{T{-3}, T{+1}}));
        BOOST_CHECK((xstd::floored_div(T{-8}, T{-3}) == xstd::div_t<T>{T{+2}, T{-2}}));

        // denom == MIN, which the postconditions need uabs for.
        //
        // The euclidean remainder is spelled -1 - min() rather than max(),
        // which is the same value only on a two's complement type. Here it is
        // max() - 1, because min() is -max() rather than -max() - 1. The
        // corresponding check in test/src/cstdlib.cpp can and does write
        // max() outright, since every type it covers is two's complement;
        // writing it that way here is what failed first, and the library was
        // right both times. Expected values for this case have to be derived
        // from the type's own bounds, not from a width.
        BOOST_CHECK((xstd::div(T{-1}, limits::min()) == xstd::div_t<T>{T{0}, T{-1}}));
        BOOST_CHECK((xstd::euclidean_div(T{-1}, limits::min()) == xstd::div_t<T>{T{+1}, static_cast<T>(T{-1} - limits::min())}));
        BOOST_CHECK((xstd::floored_div(T{-1}, limits::min()) == xstd::div_t<T>{T{0}, T{-1}}));

        // and that really is max() - 1 at this representation, not max()
        BOOST_CHECK((static_cast<T>(T{-1} - limits::min()) == static_cast<T>(limits::max() - T{1})));
}

// The symmetric range itself, stated rather than assumed. uabs is specified as
// |x| for every input, not as a two's complement bit pattern, so on a
// signed-magnitude type |min()| is simply max() - where a two's complement
// type of the same width answers max() + 1. Both are correct; that they differ
// is the point of testing this representation at all.
BOOST_AUTO_TEST_CASE_TEMPLATE(SymmetricRange, T, cpp_int_widths)
{
        using U = xstd::make_unsigned_like_t<T>;
        using limits = std::numeric_limits<T>;

        BOOST_CHECK((limits::min() == -limits::max()));
        BOOST_CHECK((xstd::uabs(limits::min()) == static_cast<U>(limits::max())));
        BOOST_CHECK((xstd::uabs(limits::max()) == static_cast<U>(limits::max())));

        // xstd::abs is *not* exercised at min() here, even though |min()| is
        // representable in T at this width and the call would be perfectly
        // well-defined. Its precondition is spelled x != min() unconditionally,
        // which is exact for a two's complement type and conservative for a
        // symmetric-magnitude one. Calling it would trip xstd's own assert
        // rather than test anything; see doc/design.md.
        BOOST_CHECK((xstd::abs(static_cast<T>(limits::min() + T{1})) == static_cast<T>(limits::max() - T{1})));
}

BOOST_AUTO_TEST_SUITE_END()
