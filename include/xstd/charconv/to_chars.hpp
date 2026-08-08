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

// The largest number of characters to_chars can write for I, at any base it
// accepts. Base 2 is the worst case. For an unsigned type that is exactly one
// character per value bit; for a signed one it is two more, not one: digits
// counts value bits only, so the magnitude of min() is 2^digits and takes
// digits + 1 binary digits, and the sign takes another. At int8_t that is the
// difference between fitting "-10000000" and not. No null terminator: to_chars
// does not write one.
template<integral_like I>
inline constexpr auto to_chars_max_size =
        static_cast<std::size_t>(std::numeric_limits<I>::digits) + (is_signed_like_v<I> ? 2 : 0);

// std::to_chars, widened to every integral-like type: two overloads on one
// name, rather than one function branching on an if constexpr.
//
// The default base is the literal 10, the way [charconv.to.chars] spells it,
// rather than a named constant. A caller reads this signature against the
// standard's, so the two should differ in nothing that is visible there.
// readability-magic-numbers is enabled for the headers and does fire on a
// default argument, so each of the three declarations carries a NOLINTNEXTLINE
// for it - the narrowest suppression there is, one check on one line, rather
// than adding 10 to the check's ignored values and losing it everywhere else in
// the library.
//
// Which one a call selects is decided by the constraints alone. This one is
// constrained on nothing beyond integral_like, and the one below adds a
// requires-expression on top of it; a conjunction subsumes its left operand, so
// wherever both are viable the delegating one is more constrained and wins
// partial ordering. Spelling this one's constraint as the negation would work
// too, but a negated atomic constraint does not subsume, so exclusivity and
// exhaustiveness would become an invariant to maintain across two edits rather
// than a property of the constraints. As written there is no gap and no
// ambiguity: every integral-like type matches this overload, and the ones the
// standard library covers match both.
//
// This one comes first for a reason outside the language. gcov's text output
// names only the first group of functions sharing a start line in a file, and
// gcovr keys its cross-translation-unit merge on those names: whatever is left
// unnamed merges with nothing. The two bodies are not symmetric in what that
// costs. This one contains a line no other translation unit can reach - the
// short-buffer return, which only the charconv test provokes - so its records
// have to merge with the copies emitted wherever else it is instantiated, such
// as the div_t formatter on a standard library without p2286. Every line of the
// delegating body runs in every instantiation that exists at all, so leaving
// that one unnamed costs nothing. Ordered the other way, the file reads 98%
// covered and the gate fails.
//
// Same output as the standard's, for the types it does not cover: integer-class
// types everywhere, and the built-in 128-bit types in the strict dialect this
// library compiles in. Digits are counted first so the result can be written
// forwards into the caller's range, which is the interface the standard
// specifies: no null terminator, and ptr one past the last character written.
//
// The magnitude is never formed. Negating the minimum value has no
// representation, and an integer-class type need not offer a wider one to
// borrow, so the remainder is taken on the negative value and its sign is
// absorbed by the direction the digit table is read in. That also means no
// make_unsigned_like is needed, which an integer-class type is only required to
// have if its author supplied one.
template<integral_like I>
// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base, see above
[[nodiscard]] constexpr auto to_chars(char* first, char* last, I value, int base = 10)
        -> std::to_chars_result
{
        assert(2 <= base and base <= 36);

        // A pointer to the literal rather than a string_view: the offset below
        // is signed, and subscripting a view would mean converting it to the
        // view's size_type first, which is a widening cast of narrow arithmetic
        // and a diagnostic of its own. modernize-avoid-c-arrays is about
        // declaring an array, which this is not.
        static constexpr auto* digits = "0123456789abcdefghijklmnopqrstuvwxyz";

        // static_cast rather than I{0}: [iterator.concept.winc]/6 is what makes
        // a zero of an integer-class type available at all, and it grants a
        // conversion - "expressions of integral type are both implicitly and
        // explicitly convertible to any integer-class type", which does not
        // exit via an exception. Braces are not a conversion but overload
        // resolution over constructors, where an initializer_list constructor
        // would win, and nothing in that subclause rules one out.
        auto const radix = static_cast<I>(base);
        auto const zero = static_cast<I>(0);

        // One decision and the two things that follow from it: whether a sign
        // has to be written, and which way the digit table is read. Both are
        // runtime values - a signed type converts positive values too, so the
        // direction belongs to the value rather than to the instantiation - but
        // this is the only place either of them consults signedness.
        //
        // The if constexpr is what makes the conditional inside affordable: it
        // exists only in the signed instantiation, which reaches both of its
        // sides, where one written at block scope would be a branch an unsigned
        // instantiation could only ever take one side of, and the coverage gate
        // counts branches per instantiation. The comparison itself needs no
        // shielding - a tautological-comparison warning is suppressed when it
        // arises from a template instantiation, on both compilers this builds
        // with, so value < zero at an unsigned I is silent.
        //
        // The return type is pinned rather than deduced because both returns
        // are braced-init-lists, which deduction has nothing to work from. The
        // bool in it is exact rather than a conversion: /7.6 pins an
        // integer-class type's relational operators to bool, and
        // exposition_only::integer_class_type transcribes that, so value < zero
        // is a bool and not a proxy standing in for one. The capture is a
        // default rather than naming value, which Clang's
        // -Wunused-lambda-capture - reached through -Weverything, and an error
        // here - would flag in the instantiation that discards the only branch
        // reading it.
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
        // Converted rather than selected with a conditional: this is at block
        // scope, where a conditional is a branch gcov records per instantiation
        // and an unsigned one has no way to reach the other side of.
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
                // A remainder is smaller than the base it came from, and the
                // stride has just turned it positive, so this indexes the table
                // and nothing else. Asserted because an integer-class type's
                // operator% is opaque to the static analyzer, which without it
                // has to assume the index reaches outside the literal.
                assert(0 <= index and index < base);
                *--out = digits[index];
                if (rest / radix == zero) {
                        break;
                }
        }
        // The one place the sign still has to be a test rather than an index or
        // a conversion, and so the one that stays under an if constexpr for the
        // reason given above.
        if constexpr (is_signed_like_v<I>) {
                if (negative) {
                        *--out = '-';
                }
        }
        return {.ptr = first + count, .ec = std::errc{}};
}

// The more constrained of the two, selected wherever the standard library
// covers I. It is that call and nothing more, so the tuned implementation and
// its format-spec behavior are what a caller gets.
//
// The constraint is the call itself: there is no standard concept for "the
// standard library converts this type", so the requires-expression names
// std::to_chars directly rather than being wrapped in a helper concept. Two
// ways it can be unsatisfied, and it has to survive both:
//
// - No overload at all, for an integer-class type. A clean deduction failure.
// - An *ambiguous* overload, which is what libstdc++ gives for __int128 in
//   strict mode: the public overload set is a set of non-template functions
//   generated per built-in width, and the 128-bit pair is gated on
//   __GLIBCXX_TYPE_INT_N_0, which the compiler only predefines outside
//   __STRICT_ANSI__. Without it the argument converts equally well to every
//   width. Ambiguity is still overload resolution failing in the immediate
//   context, so the requirement is unsatisfied rather than ill-formed.
//
// Both of those are answers rather than errors only because I is this
// template's own parameter, so the expression stays dependent until the
// constraint is checked. The same requires-expression written at a concrete
// type - as the tests want, to assert which path a type takes - has to be a
// named concept there, for exactly that reason.
//
// std::to_chars(bool) is deleted, so bool is unsatisfied here and is deleted
// below rather than silently picking up the digits overload.
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

// Deleted for the same reason the standard deletes it: bool is integral-like,
// so an unconstrained call would silently render true as "1" rather than say
// that the caller almost certainly meant something else.
// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base, see above
auto to_chars(char*, char*, bool, int = 10) -> std::to_chars_result = delete;

} // namespace xstd

#endif // XSTD_CHARCONV_TO_CHARS_HPP
