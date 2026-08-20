# Design notes

## Purpose

xstd is a header-only collection of small C++ standard-library extensions.
There is no single proposal behind it: it collects facilities suggested by
existing practice that can be implemented portably, with a standard-library
interface. The baseline is C++23, and may move to C++26 once that is common
across the tested toolchains. Consumers need no third-party dependencies.

## Principles

- **Prefer `constexpr`.** Value-oriented functions are usable during constant
  evaluation unless the standard library operation they delegate to prevents it.
- **Generalize integer support.** The standard integral concepts and traits are
  closed over built-in types; xstd's integer concepts also accept paired
  integer-class types, based on their behavior and `xstd::numeric_limits`.
- **Keep metaprogramming small.** `specialization_of`, `empty_type` and
  `conditional_data_member_t` solve local problems without a framework.
- **Make semantics explicit.** The three division functions name their rounding
  convention rather than hiding it behind `/` and `%`.
- **Stay modular and dependency-free.** Linking `xstd::xstd` adds include paths
  and the C++23 requirement, but no runtime library or transitive package.

## API shape

### Integer types

[P3701R0](https://wg21.link/P3701R0) supplies the `integer`,
`signed_integer`, and `unsigned_integer` vocabulary and its built-in arithmetic
boundary. The public concept is cv-transparent and accepts standard and extended
integer types except `bool`, `char`, `wchar_t`, `char8_t`, `char16_t`, and `char32_t`.
`signed char` and `unsigned char` remain integers, so `int8_t` and `uint8_t`
remain supported where they are aliases of those types.

Unlike the paper's closed standard-library domain, xstd also admits types that
model `integer_class`. Every accepted type must have valid `make_signed_t` and
`make_unsigned_t` transformations; this deliberately rejects an unpaired
integer-class type. The refinements use xstd's open signedness traits, preserving
concept subsumption across built-in, extended, bit-precise, and paired class
integers.

Character conversion delegates a non-`bool` standard integral type to
`std::to_chars` when it is no wider than `uint128`, which is where both standard
libraries stop; every other `integer` takes xstd's own digit generation.

`integer_class` follows the requirement order in [iterator.concept.winc], but is
an open structural concept rather than an implementation-defined set. It admits
integer types of every width instead of requiring them to be wider than every
integral type, and it deliberately checks only same-type arithmetic rather than
mixed-mode operations or common types. Its range metadata comes from the open
`xstd::numeric_limits`, whose primary template delegates to the standard trait.

When Clang exposes `_BitInt`, `<xstd/cstdint.hpp>` names the native types as
`bit_int<N>` and `bit_uint<N>`. They are aliases, not wrappers: their ABI,
conversions, promotions, and operators remain the compiler's. xstd specializes
its open limits and transformation traits for them, which makes paired widths
model `integer` even before the standard library recognizes the extension.
Whether they reach it as `std::integral` or as `integer_class` is the standard
library's call: libc++ makes `_BitInt` integral, libstdc++ does not. The public
domain currently begins at two because Clang has not yet implemented N3747's
signed `_BitInt(1)`.

### Conditional `noexcept`

All integer operations are `constexpr`. Their `noexcept` is conditional, on
whether the element type's own operations carry the specifier, because
[iterator.concept.winc] does not ask for `noexcept` anywhere — the word does not
occur in the subclause. Requiring it would narrow the extension point to
integer-class types whose authors wrote it: `absl::uint128` is two `uint64_t`
halves with no way to throw and not one `noexcept` in its header, and was turned
away. Dropping it would go too far the other way, since `std::abs` and `std::div`
*are* `noexcept` as both major implementations ship them. Conditioning keeps
both, and refuses neither. `xstd::to_chars` is unconditional in the other
direction, having no `noexcept` at all, because `std::to_chars` has none either.

The predicate is named for what it can see — a type's declarations, not its
behavior. "Integral" rather than "arithmetic", because an arithmetic category would also include floating point and this answers false for every
floating-point type; "operators", because it ranges over a type's operators
rather than its category. It is public because it *is* those six exception
specifications, and a caller asking whether one throws over their own type is
asking exactly this.

Its operations are every requirement `integer_class` states over `const`
operands, and no more. That boundary is not this library's invention:
`absl::uint128` declares each of them `constexpr` and not one of its mutating
operators, so the `const` half is what an integer-class type in the field treats
as its value surface. It is also the only half these functions can reach, each
taking and returning a value. Within it the coverage is total rather than
itemized — a list drawn from what the bodies contain has to be revised whenever a
body changes, and being over-cautious about an operation a function never
performs costs nothing, where missing one it does perform is a wrong
`noexcept(true)`.

None of this would be here if the language could deduce it. `noexcept(auto)` was
proposed against exactly this shape of problem —
[N3207](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2010/n3207.htm)
(2010), still open as
[P3166R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p3166r0.html)
(2024) — and C++23 has no such thing. The manual `noexcept(noexcept(e))` reaches
only a single-expression function, which these are not.

A `= default` function needs no stand-in and gets neither specifier: defaulted on
its first declaration it is implicitly `constexpr` if the implicit declaration
would be, and its exception specification is computed. So `div_result`'s equality and
`empty_type`'s default constructor and `<=>` write only `[[nodiscard]]`, the one
of the three with no implicit form. Neither is restated for symmetry, because the
two fail differently: a `constexpr` that cannot hold is refused where a constant
expression needs it, while
[P1286R2](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1286r2.html)
(C++20) removed the rule that deleted such a defaulted function, so an over-broad
`noexcept` buys a `std::terminate` in place of a diagnostic.

### The 128-bit aliases

They are spelled `int128` and `uint128`, without the `_t` that `<cstdint>`'s
exact-width names carry. C reserves typedef names beginning with `int` or `uint`
and ending in `_t` for future additions to `<stdint.h>`. The reservation does not
reach into a user-defined namespace, but `<xstd/cstdint.hpp>` exists precisely
because the standard header lacks the type, so the day it gains one is the day
the two spellings collide. Boost.Int128 renamed the same way during its review.

Their two associations live in `<xstd/cstdint/int128.hpp>`, beside the aliases
and not inside the traits; `<xstd/cstdint.hpp>` is the umbrella that exports
them. This is where `test/include/xstd/test/` puts Abseil's and
Boost.Int128's: the header that introduces a pair registers it, so whoever can
name the type has the specializations in scope and cannot reach the type through
a translation unit that would answer differently. It also leaves `make_signed`
and `make_unsigned` knowing about no type in particular. What the aliases still
cannot do is demonstrate the widening on their own, being specialized for by a
header of this library's; the exact-width test lists therefore also carry
Boost.Int128 and `absl::int128`, which no shipped header names, and the two
associations written for each in `test/include/xstd/test/` are the whole of what
a user has to write. Both dependencies are optional; see
[CONTRIBUTING.md](../CONTRIBUTING.md).

The two are not interchangeable, which is why both are here. Boost annotates its
operations and Abseil annotates none, so they land on opposite sides of
`nothrow_integer_operators`. They differ once more: `absl::int128`'s `operator/`
and `operator%` are `constexpr` only where a 128-bit intrinsic backs them, and
out-of-line otherwise. That is the one place these tests assert something about
the target rather than the type. The whole-surface battery in
`test/src/cstdlib.cpp` runs over it everywhere, constant-evaluated where the
intrinsic is there and checked at run time where it is not; its place in the
exact-width lists is conditional on `ABSL_HAVE_INTRINSIC_INT128` instead, those
cases constant-evaluating everything. Only the three divisions object at all, and
those are exactly what the battery covers.

### Traits and concepts

The type utilities intentionally remain narrow:

- `is_specialization_of` and `specialization_of` recognize specializations of
  class templates whose parameters are types.
- `is_signed` and `is_unsigned` are open counterparts of the standard traits.
- `empty_type` and `conditional_data_member_t` support optional
  `[[no_unique_address]]` storage. Its tag defaults to `void`, so `empty_type<>`
  also serves the uses with nothing to keep distinct, such as the base
  `numeric_limits` inherits outside the domain the standard trait can name.
- `to_underlying` forwards a plain enum and preserves one wrapped in
  `std::integral_constant`.
- `nothrow_integer_operators` answers whether the conditional `noexcept` holds.

A concept spelling is provided when the standard library has an analogous
concept; otherwise the trait is the interface. `nothrow_integer_operators` is
the one concept with no trait beside it, having no standard trait to mirror, and
is spelled the way a caller writes it: inside a `noexcept`.

Which decides where the `is` goes, and the subclause is no guide there. Its
concept is *is-integer-like* — the only concept in the library or the standard
whose name starts with `is`, because it is exposition-only and answers to nobody.
The convention outside it is the opposite and is worth following: `is` marks a
trait, as in `std::is_integral`, and a concept goes bare, as in `std::integral`.
So the concept here is `integer`, bare, rather than an `is_integer`
transliterating a hyphenated name that was never meant to be public; where a trait
does stand beside a concept, as `is_signed` does beside `signed_integer`, the
`is` is what marks which is which.

### `to_underlying`

The [original 2016 sketch](ideas.md#1-convenient-underlying-types-for-scoped-enums)
motivated `to_underlying` with scoped enums used as named tuple and array
indices. Rein Halbersma developed the idea and initial usage evidence with
Walter E. Brown; JeanHeyd Meneide then authored
[P1682R1](https://wg21.link/P1682R1) and carried `std::to_underlying` through
WG21 for C++23. P1682's acknowledgements record those roles. The xstd overload
complements it: given an enum value wrapped in `std::integral_constant`, it
returns an `integral_constant` of the underlying type, preserving the value at
the type level.

A second overload forwards a plain enum to `std::to_underlying`, so `xstd::` is
one spelling over both forms rather than a name a caller has to remember to
switch away from for the unwrapped case. It is an overload rather than a
using-declaration because the constraint is then written where it applies, as
the wrapped one writes its own.

### Character conversion

`xstd::to_chars` delegates every non-`bool` standard integral type directly to
the standard library, so callers get its tuned implementation. For an xstd
`integer` that `std::to_chars` does not cover, the digits are produced here to
the same specification, bases 2 through 36 and `value_too_large` included. The
default base is written as the literal `10`, as [charconv.to.chars] writes it, so the two
signatures read alike; `readability-magic-numbers` is suppressed a line at a time
rather than taught to ignore `10` throughout.

The digits come off the unsigned counterpart of `I`, the only type in which the
magnitude of `min()` is representable, and everything after that is unsigned
arithmetic. Staying in `I` would work, `%` yielding a remainder of the value's
own sign that a digit table read backwards absorbs, but it pays for the sign at
every digit rather than once and forces both loops to test `rest / radix == 0`
where an unsigned magnitude tests `rest >= radix` — one division per digit rather
than two, and for an integer-class type a division is a call. libstdc++ makes the
same reduction in `__to_chars_i`, and the loop shape here is its
`__detail::__to_chars`.

Neither loop counts characters. The first walks a pointer forward from where the
most significant digit goes, and a step onto `last` is the short buffer; the
second walks it back down, writing as it goes. The buffer's end is the walk's own
bound rather than a count, which keeps the comparison between two pointers and
leaves no signed-to-unsigned mismatch to answer; the one place a length is asked
for at all is the check ahead of the walk, which spells it `std::distance`. The
sign is written last, the walk stepping back into the one position it reserved
and no digit claimed; writing it first is what obliges libstdc++ to leave a `'-'`
in a buffer it then reports as too small. That the walk cannot start before there is
room for the first digit and the sign is why `value_too_large` has two returns,
both covered per type since gcov records them per instantiation.

The overloads state the split literally. The delegating overload accepts
`std::integral` except `bool`; the fallback accepts `integer` only when a
`std::to_chars` call is not well-formed. That detection is written directly in
the fallback's constraint; it stays an answer rather than an error because `I`
keeps the expression dependent until the constraint is checked. Between the two
templates, a deleted `bool` overload mirrors the standard explicitly rather than
leaving the rejection as an incidental result of their constraints.

Their order in the header is reading order: the standard's own domain, then the
deleted `bool`, then the fallback for what it does not cover. It once could not
be. gcov names only the
first group of functions sharing a start line in a file, and gcovr keyed its
cross-translation-unit merge on those names,
so an unnamed group merged with nothing; the digits body contains lines no other
translation unit can reach — the short-buffer returns — so it had to be the named
one, and ordered the other way the file measured 98% and the coverage gate
failed. The same sensitivity was why the digits body kept its sign and magnitude
as two plain declarations. Passing `--merge-lines` to gcovr merges a template's
instantiations by line rather than by name, which dissolved that constraint and
with it the reason the digits overload came first.

Two places the standard library does not cover are worth separating. Integer-class
types are never covered, `to_chars` being specified over the built-in integer
types. But the *built-in* 128-bit types are not covered either in this dialect:
libstdc++ gates its 128-bit pair on `__GLIBCXX_TYPE_INT_N_0`, predefined only
outside `__STRICT_ANSI__`, so with `CMAKE_CXX_EXTENSIONS OFF` the overload is
absent and the call is *ambiguous*. Detection therefore has to survive an
ambiguity rather than a clean deduction failure, which it does: overload
resolution failing is still failing in the immediate context.

`<format>` goes the other way for the same underlying reason, libstdc++ adding a
`formatter<__int128, CharT>` *only* under `__STRICT_ANSI__`. So the three
facilities disagree about one type along different axes: `<format>` works only in
the strict dialect, `<charconv>` only outside it, and `<ostream>` in neither.
`bool` is outside both conversion constraints.

### Formatting

`<xstd/format.hpp>` formats a `div_result` as `(quotient, remainder)`, through two partial
specializations of `std::formatter`. `div_result` is tuple-like, so where the standard
library can format a tuple of the element type the specialization to use inherits
`std::formatter<std::tuple<I const&, I const&>>` and hands it `std::tie` — the
standard's own rendering, with no intermediate string.

That delegation can be missing in two ways that do not coincide: the element type
may not be formattable, which is the Microsoft STL's 128-bit classes, or tuple
formatting itself may be missing, p2286 having reached libstdc++ only in GCC 15.
Testing `formattable` on the *tuple* covers both with one predicate. Where it
does not hold, the other specialization renders the members through
`xstd::to_chars` into an inherited string formatter, so `div_result` formats on every
implementation and for either signedness — for every element type `xstd::to_chars`
itself covers, which is to say every one with an unsigned counterpart.

The choice is left to partial ordering, on the same footing as `to_chars`'s
overloads: the tuple one requires the base it inherits, spelled where it applies,
on top of the counterpart the other asks for, so its constraints are a superset
and it wins wherever both are viable. Both produce `(quotient, remainder)`, so which runs is
not observable in the output — only in the spec grammar, `parse()` being
inherited, which is why the README says so.

xstd specializes `std::formatter` only for `div_result`, which is program-defined. The
128-bit types are not xstd's to specialize for: they are built-ins or
standard-library types, which [namespace.std]/2 does not cover. There is no
stream inserter either — a built-in has no associated namespace for ADL, and a
standard-library type's is one no program may add to. Boost.Test, the only
consumer that needed it, asks through `print_log_value`, which the tests
specialize directly.

## Integer division

`div_result` is xstd's common result for three complete integer-division
conventions: truncating `div`, Euclidean `div_euclid`, and floored `div_floor`.
Its result vocabulary follows [P3724R4](https://wg21.link/P3724R4), while xstd
intentionally limits the operation family to these established conventions and
extends all three across its broader `integer` domain.

```cpp
template<xstd::integer I>
struct div_result
{
    I quotient;
    I remainder;
};
```

Every operation returns quotient and remainder together and preserves:

```text
numerator == denominator * quotient + remainder
```

| Function | Quotient rule | Remainder rule |
|---|---|---|
| `div` | toward zero | zero or numerator's sign |
| `div_euclid` | chosen for a nonnegative remainder | `0 <= remainder < abs(denominator)` |
| `div_floor` | toward negative infinity | zero or denominator's sign |

```cpp
constexpr auto result = xstd::div_floor(-8, 3);
static_assert(result.quotient == -3);
static_assert(result.remainder == 1);

auto const [quotient, remainder] = xstd::div_euclid(-8, 3);
auto const [q, r] = xstd::div_floor(-8, 3);
```

xstd adopts the relevant P3724 result vocabulary and rounding semantics, not the
complete proposal. No other rounding modes are part of this change. All three
operations support signed, unsigned, extended, bit-precise, and paired
integer-class types; for unsigned types all three conventions coincide.

## Requirements and evolution

xstd requires a conforming C++23 compiler, and the CMake project CMake 3.28 or
later, exporting the header-only `xstd::xstd` target. Consumers build neither the
tests nor their dependencies. New facilities should stay small, portable and
motivated by current practice; one that enters the standard library can be
retired, and the baseline can advance to C++26 once that is a practical default.

## CI policy

The matrix follows the channel model used by
[apt.llvm.org](https://apt.llvm.org/): **stable** is the established release,
**qualification** the newest release still being qualified, and **development**
the current development branch. These are tested across the three main compiler
families, the three main standard libraries and the three main desktop platforms,
with MinGW, Apple Clang and Clang-CL filling out the remaining combinations.
Development jobs are required where a usable development compiler exists; the
README records the versions currently assigned to each channel. Building the
tests requires the dependencies documented in
[CONTRIBUTING.md](../CONTRIBUTING.md).
