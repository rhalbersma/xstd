//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONCEPTS_EXPOSITION_ONLY_HPP
#define XSTD_CONCEPTS_EXPOSITION_ONLY_HPP

#include <compare>     // strong_ordering
#include <concepts>    // constructible_from, integral, regular, same_as, three_way_comparable
#include <cstddef>     // size_t
#include <limits>      // numeric_limits
#include <type_traits> // remove_cv_t
#include <utility>     // declval

// Internal concepts used to define xstd's public numeric traits and concepts.
namespace xstd::exposition_only {

// Structural form of the integer-class requirements in [iterator.concept.winc],
// stated as the subclause states them: for an object of the type, and so for a
// type carrying no cv-qualification. integer_class_type below is the spelling
// to use.
//
// None of these expressions is required to be noexcept, because the subclause
// never says so: the word does not appear in it. The only thing it says about
// throwing is /6, that conversions "do not exit via an exception" - a statement
// about what happens at run time, not about how the conversion is declared, and
// so not something a requires-expression can check. Requiring the specifier
// anyway would narrow this to integer-class types whose authors annotated them,
// which is a different and much smaller set: absl::uint128, to name the obvious
// one, does not use the word once in its header, though its two uint64_t halves
// have no way to throw. What the annotations are worth is decided where it can
// be acted on - the arithmetic surface propagates them, see
// exposition_only::nothrow_arithmetic.
template<class I>
concept unqualified_integer_class_type =
        (not std::integral<I>) and
        requires { sizeof(I); } and
        // [iterator.concept.winc] first specifies conversions between integral
        // and integer-class types.
        std::constructible_from<I, int> and
        // It then specifies increment/decrement, unary operators, compound
        // assignment, and non-assignment binary operators, in that order.
        requires (I a) {
                { a++ } -> std::same_as<I>;
                { a-- } -> std::same_as<I>;
                { ++a } -> std::same_as<I&>;
                { --a } -> std::same_as<I&>;
        } and
        requires (I const a) {
                { static_cast<I>(+a) } -> std::same_as<I>;
                { static_cast<I>(-a) } -> std::same_as<I>;
                { static_cast<I>(~a) } -> std::same_as<I>;
                // bool exactly, not merely convertible to it. /7.3 ranges over
                // unary-operator, which is `* & + - ! ~`, excludes `&` by name
                // and leaves `*` with nothing an integer can apply it to - so
                // these four are its whole scope - and then pins the result in
                // one sentence: "if @x has type bool, so too does @a; if @x has
                // type B(I), then @a has type I". The first half of that
                // sentence is there for `!` alone, the one operator of the four
                // whose result on an integer is not the integer type again.
                // convertible_to<bool> would leave it with nothing to say.
                { not a } -> std::same_as<bool>;
        } and
        requires (I a, I const b) {
                { a += b } -> std::same_as<I&>;
                { a -= b } -> std::same_as<I&>;
                { a *= b } -> std::same_as<I&>;
                { a /= b } -> std::same_as<I&>;
                { a %= b } -> std::same_as<I&>;
                { a &= b } -> std::same_as<I&>;
                { a |= b } -> std::same_as<I&>;
                { a ^= b } -> std::same_as<I&>;
        } and
        requires (I a, std::size_t n) {
                { a <<= n } -> std::same_as<I&>;
                { a >>= n } -> std::same_as<I&>;
        } and
        requires (I const a, I const b) {
                { static_cast<I>(a + b) } -> std::same_as<I>;
                { static_cast<I>(a - b) } -> std::same_as<I>;
                { static_cast<I>(a * b) } -> std::same_as<I>;
                { static_cast<I>(a / b) } -> std::same_as<I>;
                { static_cast<I>(a % b) } -> std::same_as<I>;
                { static_cast<I>(a & b) } -> std::same_as<I>;
                { static_cast<I>(a | b) } -> std::same_as<I>;
                { static_cast<I>(a ^ b) } -> std::same_as<I>;
        } and
        requires (I const a, std::size_t n) {
                { static_cast<I>(a << n) } -> std::same_as<I>;
                { static_cast<I>(a >> n) } -> std::same_as<I>;
        } and
        requires (I const a, I const b) {
                // Pinned for the same reason, by the last clause of /7.6:
                // `a @ b` has whatever type `x @ y` has once that type is
                // neither B(I) nor B(I2), which for these two is bool and
                // strong_ordering.
                //
                // The regularity and ordering below do not make the first line
                // redundant: std::equality_comparable asks only that `a == b`
                // be boolean-testable, and that weaker requirement is all the
                // four relational operators are held to here, since no line
                // spells them out. The second line is a restatement -
                // three_way_comparable<I, strong_ordering> already forces this
                // exact type through compares-as - kept because transcribing
                // the subclause is what this concept is for, and /7.6 is where
                // the subclause says it.
                { a == b } -> std::same_as<bool>;
                { a <=> b } -> std::same_as<std::strong_ordering>;
        } and
        // Finally come regularity and ordering, value-initialization, and
        // numeric_limits.
        std::regular<I> and
        std::three_way_comparable<I, std::strong_ordering> and
        requires {
                { static_cast<I>(0) } -> std::same_as<I>;
        } and
        std::numeric_limits<I>::is_specialized and
        std::numeric_limits<I>::is_integer;

// The same requirements, asked of a type that may be cv-qualified.
//
// /11 speaks of "every (possibly cv-qualified) integer-class type", and the
// standard's own category traits answer the same for int as for int const, so
// the widening has to as well. It cannot do that by asking the requirements
// above of a qualified type directly: they are stated for an object that can be
// assigned and incremented, and `++a`, `a += b` and std::regular all fail on a
// const one. Without this, integral_like admitted `int const` and turned away
// `absl::uint128 const` - a difference between the two branches that nothing in
// the subclause asks for. Stripped once here rather than in each requirement.
template<class I>
concept integer_class_type = unqualified_integer_class_type<std::remove_cv_t<I>>;

// Whether the arithmetic surface can pass a noexcept on. True for every
// integral type, and for an integer-class type whose author wrote the
// specifier; false for one that merely happens not to throw, which is not
// something a declaration can be asked.
//
// One predicate for all six functions rather than one apiece: they draw on the
// same handful of operations, and a function that reports noexcept(false) over
// an operation it does not itself perform has only been too cautious. Being
// wrong in the other direction is what matters, and this cannot be.
template<class T>
inline constexpr auto nothrow_arithmetic =
        noexcept(static_cast<T>(0)) and
        noexcept(static_cast<T>(-std::declval<T const&>())) and
        noexcept(static_cast<T>(std::declval<T const&>() + std::declval<T const&>())) and
        noexcept(static_cast<T>(std::declval<T const&>() - std::declval<T const&>())) and
        noexcept(static_cast<T>(std::declval<T const&>() / std::declval<T const&>())) and
        noexcept(static_cast<T>(std::declval<T const&>() % std::declval<T const&>())) and
        noexcept(std::declval<T const&>() < std::declval<T const&>());

} // namespace xstd::exposition_only

#endif // XSTD_CONCEPTS_EXPOSITION_ONLY_HPP
