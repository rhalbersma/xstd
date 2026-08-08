//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CHARCONV_TO_CHARS_HPP
#define XSTD_CHARCONV_TO_CHARS_HPP

#include <xstd/concepts/integral_like.hpp>     // integral_like
#include <xstd/type_traits/is_signed_like.hpp> // is_signed_like_v
#include <cassert>                             // assert
#include <charconv>                            // to_chars, to_chars_result
#include <concepts>                            // same_as
#include <cstddef>                             // ptrdiff_t, size_t
#include <limits>                              // numeric_limits
#include <system_error>                        // errc
#include <utility>                             // cmp_less, pair

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

        // A pointer rather than a string_view: the offset below is signed, and
        // a view's size_type would want a widening cast to subscript.
        static constexpr auto* digits = "0123456789abcdefghijklmnopqrstuvwxyz";

        // static_cast rather than I{0}: /6 grants the conversion, where braces
        // are overload resolution an initializer_list constructor would win.
        auto const radix = static_cast<I>(base);
        auto const zero = static_cast<I>(0);

        // Whether a sign is written and which way the table is read. Under an
        // if constexpr: gcov counts branches per instantiation.
        auto const [negative, stride] = [&] -> std::pair<bool, std::ptrdiff_t> {
                if constexpr (is_signed_like_v<I>) {
                        auto const is_negative = value < zero;
                        return {is_negative, is_negative ? -1 : 1};
                } else {
                        return {false, 1};
                }
        }();

        auto count = std::size_t{0};
        for (auto rest = value;; rest = static_cast<I>(rest / radix)) {
                ++count;
                if (rest / radix == zero) {
                        break;
                }
        }
        // Converted rather than selected: a conditional here would be a branch
        // an unsigned instantiation could only take one side of.
        count += static_cast<std::size_t>(negative);

        if (std::cmp_less(last - first, count)) {
                return {.ptr = last, .ec = std::errc::value_too_large};
        }

        auto* out = first + count;
        for (auto rest = value;; rest = static_cast<I>(rest / radix)) {
                // The stride absorbs the remainder's sign; the value's own sign
                // is written once, below.
                auto const digit = static_cast<int>(rest % radix);
                auto const index = stride * digit;
                // In range by construction, asserted because operator% on an
                // integer-class type is opaque to the analyzer.
                assert(0 <= index and index < base);
                *--out = digits[index];
                if (rest / radix == zero) {
                        break;
                }
        }
        // The one place the sign is a test rather than an index, so the one
        // that stays under an if constexpr, for the reason above.
        if constexpr (is_signed_like_v<I>) {
                if (negative) {
                        *--out = '-';
                }
        }
        return {.ptr = first + count, .ec = std::errc{}};
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
