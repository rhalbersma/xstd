//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CHARCONV_TO_CHARS_HPP
#define XSTD_CHARCONV_TO_CHARS_HPP

#include <xstd/concepts/integral_like.hpp>         // integral_like
#include <xstd/cstdlib/unsigned_abs.hpp>           // unsigned_abs
#include <xstd/type_traits/is_signed_like.hpp>     // is_signed_like_v
#include <xstd/type_traits/make_unsigned_like.hpp> // make_unsigned_like_t
#include <cassert>                                 // assert
#include <charconv>                                // to_chars, to_chars_result
#include <concepts>                                // same_as
#include <cstddef>                                 // ptrdiff_t, size_t
#include <limits>                                  // numeric_limits
#include <system_error>                            // errc
#include <utility>                                 // pair

namespace xstd {

// Worst case over every base, so base 2: one character per value bit, and two
// more when signed, since |min()| needs digits + 1 of them plus the sign.
template<integral_like I>
inline constexpr auto to_chars_max_size =
        static_cast<std::size_t>(std::numeric_limits<I>::digits) + (is_signed_like_v<I> ? 2 : 0);

// The digits implementation, for the types std::to_chars does not cover, less
// constrained than the one below so a call prefers the standard's.
template<integral_like I>
// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base, see above
[[nodiscard]] constexpr auto to_chars(char* first, char* last, I value, int base = 10)
        -> std::to_chars_result
{
        assert(2 <= base and base <= 36);

        // A pointer rather than a string_view: the digits are subscripted, and
        // a view would add a size no one asks for.
        static constexpr auto* digits = "0123456789abcdefghijklmnopqrstuvwxyz";

        // The digits come off the unsigned counterpart, where the magnitude of
        // min() is representable and the loops below can stop at the radix
        // rather than at zero. static_cast rather than U{0}: /6 grants the
        // conversion, where braces are overload resolution an initializer_list
        // constructor would win.
        using U = make_unsigned_like_t<I>;
        auto const radix = static_cast<U>(base);

        // Sign and magnitude, negated once here rather than once per digit.
        // Under an if constexpr: gcov counts branches per instantiation, and
        // unsigned_abs is spelled for signed types alone.
        auto const [negative, magnitude] = [&] -> std::pair<bool, U> {
                if constexpr (is_signed_like_v<I>) {
                        return {value < static_cast<I>(0), unsigned_abs(value)};
                } else {
                        return {false, value};
                }
        }();

        // Converted rather than selected: a conditional here would be a branch
        // an unsigned instantiation could only take one side of.
        auto const sign_width = static_cast<std::ptrdiff_t>(negative);

        // Every value writes a digit, and a negative one a sign before it.
        if (last - first < 1 + sign_width) {
                return {.ptr = last, .ec = std::errc::value_too_large};
        }

        // The most significant digit's position, one past the sign when there
        // is one, then a step per further digit. Claiming a position outside
        // the buffer is the short buffer, so the walk is its own bound rather
        // than a count compared against one - which is also what keeps the
        // comparison between two pointers instead of between a size and a
        // difference of them.
        auto* out = first + sign_width;
        for (auto rest = magnitude; rest >= radix; rest = static_cast<U>(rest / radix)) {
                if (++out == last) {
                        return {.ptr = last, .ec = std::errc::value_too_large};
                }
        }

        // Taken while out still means the last digit, before the write below
        // walks it back down to where it started.
        auto const result = std::to_chars_result{.ptr = out + 1, .ec = std::errc{}};

        auto rest = magnitude;
        for (; rest >= radix; rest = static_cast<U>(rest / radix)) {
                *out-- = digits[static_cast<std::size_t>(rest % radix)];
        }
        // What the loop leaves is a single digit, so the last one is written
        // here rather than as a break in the middle. No decrement: for a value
        // without a sign it would step below first.
        *out = digits[static_cast<std::size_t>(rest)];

        // Written last because the walk runs backwards, into the one position
        // it reserved and no digit claimed. The one place the sign is a test
        // rather than an offset, so the one that stays under an if constexpr,
        // for the reason above.
        if constexpr (is_signed_like_v<I>) {
                if (negative) {
                        *first = '-';
                }
        }
        return result;
}

// The standard's own call wherever it covers I, constrained on that call for
// want of a concept. An ambiguous overload leaves it unsatisfied too.
template<integral_like I>
        requires requires (char* p, I value, int base) {
                { std::to_chars(p, p, value, base) } -> std::same_as<std::to_chars_result>;
        }
// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base, see above
[[nodiscard]] constexpr auto to_chars(char* first, char* last, I value, int base = 10)
        -> std::to_chars_result
{
        assert(2 <= base and base <= 36);
        return std::to_chars(first, last, value, base);
}

// Deleted as the standard deletes it, rather than letting bool reach the
// digits overload and render true as "1".

// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base, see above
auto to_chars(char*, char*, bool, int = 10) -> std::to_chars_result = delete;

} // namespace xstd

#endif // XSTD_CHARCONV_TO_CHARS_HPP
