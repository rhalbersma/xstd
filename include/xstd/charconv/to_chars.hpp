//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CHARCONV_TO_CHARS_HPP
#define XSTD_CHARCONV_TO_CHARS_HPP

#include <xstd/concepts/has_unsigned_counterpart.hpp> // has_unsigned_counterpart
#include <xstd/concepts/integral_like.hpp>            // integral_like
#include <xstd/cstdlib/div.hpp>                       // div
#include <xstd/cstdlib/unsigned_abs.hpp>              // unsigned_abs
#include <xstd/type_traits/is_signed_like.hpp>        // is_signed_like_v
#include <xstd/type_traits/make_unsigned_like.hpp>    // make_unsigned_like_t
#include <cassert>                                    // assert
#include <charconv>                                   // to_chars, to_chars_result
#include <concepts>                                   // same_as
#include <cstddef>                                    // ptrdiff_t, size_t
#include <iterator>                                   // distance, next
#include <limits>                                     // numeric_limits
#include <system_error>                               // errc
#include <type_traits>                                // is_same_v, remove_cv_t

// Named, not written inline, so each requires-clause below stays one line and one conjunction.
namespace xstd::exposition_only {

template<class I>
concept std_to_chars_covers = requires (char* p, I value, int base) {
        { std::to_chars(p, p, value, base) } -> std::same_as<std::to_chars_result>;
};

} // namespace xstd::exposition_only

namespace xstd {

// Worst case is base 2: one character per value bit, and two more when signed.
template<integral_like I>
        requires (not std::is_same_v<std::remove_cv_t<I>, bool>)
inline constexpr auto to_chars_max_size =
        static_cast<std::size_t>(std::numeric_limits<I>::digits) + (is_signed_like_v<I> ? 2 : 0);

// The standard's own call where it covers I; an ambiguous overload leaves this unsatisfied.
template<integral_like I>
// The counterpart is carried unneeded, to stay a superset of the digits overload's.
        requires has_unsigned_counterpart<I> and exposition_only::std_to_chars_covers<I>
// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base, see above
[[nodiscard]] constexpr auto to_chars(char* first, char* last, I value, int base = 10)
        -> std::to_chars_result
{
        assert(2 <= base and base <= 36);
        return std::to_chars(first, last, value, base);
}

// Deleted as the standard deletes it: bool is not a 1-bit integer, and true is no "1".
// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base, see above
auto to_chars(char*, char*, bool, int = 10) -> std::to_chars_result = delete;

// For the types the two above miss; less constrained, so a call prefers the standard's.
template<integral_like I>
// The digits come off the counterpart, so it is a constraint rather than a body.
        requires has_unsigned_counterpart<I>
// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base, see above
[[nodiscard]] constexpr auto to_chars(char* first, char* last, I value, int base = 10)
        -> std::to_chars_result
{
        assert(2 <= base and base <= 36);

        // A pointer rather than a string_view: these are only ever subscripted.
        static constexpr auto* digits = "0123456789abcdefghijklmnopqrstuvwxyz";

        // The unsigned counterpart holds |min()| and lets both loops stop at the radix.
        using U = make_unsigned_like_t<I>;
        auto const radix = static_cast<U>(base);

        // Reduced once here, not per digit; both are well-formed and branchless when unsigned.
        auto const zero = static_cast<I>(0);
        auto const magnitude = xstd::unsigned_abs(value);
        auto const negative = value < zero;

        // Converted rather than selected: a conditional would be one-sided when unsigned.
        auto const sign_width = static_cast<std::ptrdiff_t>(negative);

        // Every value writes a digit, and a negative one a sign before it.
        if (std::distance(first, last) < sign_width + 1) {
                return {.ptr = last, .ec = std::errc::value_too_large};
        }

        // The walk claims a position per digit, so a step onto last is the short buffer.
        auto* out = first + sign_width;
        // "rest / radix" not "/=": absl::int128 is constexpr on the first only.
        for (auto rest = magnitude; rest >= radix; rest = rest / radix) {
                if (++out == last) {
                        return {.ptr = last, .ec = std::errc::value_too_large};
                }
        }

        // Taken while out still means the last digit, before the write walks it back down.
        auto const result = std::to_chars_result{.ptr = std::next(out), .ec = std::errc{}};

        // One divmod per digit, named by the library's own truncated division.
        auto rest = magnitude;
        while (rest >= radix) {
                auto const [quot, rem] = xstd::div(rest, radix);
                *out-- = digits[static_cast<std::size_t>(rem)];
                rest = quot;
        }
        // The loop leaves a single digit. No decrement: unsigned, it would step below first.
        *out = digits[static_cast<std::size_t>(rest)];

        // Back into the position the walk reserved; here the sign is a test, not an offset.
        if constexpr (is_signed_like_v<I>) {
                if (negative) {
                        *--out = '-';
                }
        }
        return result;
}

} // namespace xstd

#endif // XSTD_CHARCONV_TO_CHARS_HPP
