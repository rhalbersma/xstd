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
  closed over built-in types; xstd's `_like` counterparts also accept
  integer-class types, on their behavior and `std::numeric_limits`.
- **Keep metaprogramming small.** `specialization_of`, `empty_type` and
  `conditional_data_member_t` solve local problems without a framework.
- **Make semantics explicit.** The three division functions name their rounding
  convention rather than hiding it behind `/` and `%`.
- **Stay modular and dependency-free.** Linking `xstd::xstd` adds include paths
  and the C++23 requirement, but no runtime library or transitive package.

## API shape

### Integer-like types

`integral_like`, `signed_integral_like` and `unsigned_integral_like` widen the
standard concepts to structurally recognized integer-class types, and the `_like`
traits follow the same rule. Built-in integers need no customization; a
user-defined signed/unsigned pair supplies the opposite `make_signed_like` and
`make_unsigned_like` specializations. `has_unsigned_counterpart` is where the
integer functions ask whether that has been done, so a type arriving without it
is turned away by a constraint rather than inside a body.

The widening is cv-transparent on both branches. That takes doing only on the
integer-class one: [iterator.concept.winc] states its requirements for an object
of the type, so a `const` type fails at `++a`, at `a += b` and at `std::regular`.
Nothing in the subclause wants that difference — /11 speaks of "every (possibly
cv-qualified) integer-class type" — so the qualification comes off once, in a
defaulted second template parameter that binds the type the body is written over.
`nothrow_integral_operators` uses the same spelling: `const` would have survived
unaided, every row being stated over a `const` operand, but `volatile` splits the
two branches, a built-in answering a volatile operand on the language's own
operators where a class type's `const` members answer none of them.

Where the subclause constrains a result type the concept asks for that type, with
one exception: /7.6's `bool` for the comparisons is left to `std::regular` and
`std::three_way_comparable`, which ask only for boolean-testable results. Nothing
here can tell the difference, every use being a contextual conversion. The
language draws the line in one place anyway — a rewritten `!=` needs its
`operator==` to return exactly `bool` ([over.match.oper]) — so a type whose
equality returns a proxy must write `!=` out, and then the concept admits it.

That the concept asks for `I` exactly, rather than for something a `static_cast<I>`
could reach, is pinned by a pair of fixtures in `test/include/xstd/test/` that are
one class template at one storage type, differing in /7.6 alone: the conforming
one is asserted to satisfy `integer_class_type` and the proxy-returning one to
fail it. Both halves are the test. A failing assertion on its own would hold just
as well for a fixture that had drifted out of conformance somewhere else entirely,
and would go on holding after the clause it was written for had been relaxed.

A template parameter is named for its concept: `I` under `integral_like`, `S`
under `signed_integral_like`. The letters carry the constraint into the body,
where `S{-1}` reads as something the type can hold and `I{-1}` would not — which
is the line `div` takes care over, its `static_cast<I>(-1)` naming `max()` rather
than a value below zero.

The arithmetic functions use one constrained template per operation, over
`integral_like` rather than its signed half, covering every integer-like width of
either signedness without families such as `abs`, `labs`, `llabs` and `imaxabs`.
A function returns the argument type rather than a promoted one, and a
two-argument function requires both arguments to have the same type.

Over an unsigned type four of the six are the function they already were and one
is the identity, but each says so rather than leaving it to a dead adjustment.
`abs` and `unsigned_abs` share a shape, which is what says they coincide there
and differ only in return type. `euclidean_div` and `floored_div` return
`xstd::div`'s answer outright: a truncated remainder carries the numerator's
sign, so an unsigned one is already nonnegative and already agrees with its
denominator. Left to the adjustment, the floored case would hold only by way of
the nonzero-denominator precondition above it, which is a proof a reader has to
reconstruct. Neither unsigned branch asserts its convention's postcondition, an
assertion there being unable to fail: `sign` branches on the same
`is_unsigned_like_v` these do, so over an unsigned type it answers 0 or 1 by
construction rather than by anything the division established — it could not
even catch a `numeric_limits` specialization that lied, being misled by the same
predicate. What is left to check, that the remainder is smaller than the
denominator, `xstd::div` asserts already.

`sign` branches for a different reason: the difference of two
comparisons is correct as written, but the second is one the answer can never
depend on, and for an integer-class type a comparison is a call — and `sign` is
on the path of `div`'s postconditions and `floored_div`'s adjustment.

`bool` is integral-like, and unsigned-like at that, so the widened constraint
admits it; all six delete it, as `to_chars` does. The deletions are now there for
what they say rather than for what they prevent: `abs(true)` would answer `true`
and `sign(true)` would answer `1`, and neither is an answer worth giving.

They used to be load-bearing for a second reason. `unsigned_abs` forms
`make_unsigned_like_t<I>` in its body, where `make_unsigned_like<bool>` is the
empty primary and the failure is no longer in the immediate context, so without
the deletion the call was ill-formed rather than unsatisfied. That is what
`has_unsigned_counterpart` now asks for up front, and `bool` fails it like any
other type with no counterpart, so the deletions no longer carry that weight
alone.

The concept exists because the reasoning above generalizes past `bool`, where a
deletion cannot follow. A signed integer-class type whose counterpart the user has
not registered satisfies `integral_like`, and `unsigned_abs` and `to_chars` would
then form a `make_unsigned_like_t` that does not exist. `to_chars` was the worse
of the two: constrained on `integral_like` alone while its body needed the
counterpart, it answered *yes* to a detection idiom for a call that was ill-formed,
which defeats the fallback such an idiom exists to select. Both now require it, and
so the failure is a plain unsatisfied constraint. `to_chars`'s delegating overload
asks for it without needing it, so that its constraints stay a superset of the
digits overload's and subsumption still orders the two; the two
`std::formatter<div_t<I>>` specializations are paired the same way.

The three divisions are constrained on `integral_like` alone, and deliberately.
They never form the counterpart to compute with — `/` and `%` are all their bodies
want, and the subclause supplies both. It appears in exactly one place, the
assertion that the remainder is smaller than the denominator, which is written
over magnitudes because `|MIN|` fits in no type but the counterpart. Requiring it
in the signature would let one debug-only diagnostic decide what `div` divides, in
release builds as much as debug ones, which is the assertion setting the contract
rather than checking it. So that assertion is asked only where it can be said:

```cpp
if constexpr (has_unsigned_counterpart<I>) {
        assert(xstd::unsigned_abs(rT) < xstd::unsigned_abs(denom));
}
```

An `if constexpr` discards the branch without instantiating it, and does so on the
same terms with `NDEBUG` and without, so the mode-dependent compilation this was
first written to cure does not come back. The bound then goes unchecked for a type
with no counterpart, which is the honest cost: there is no way to compare those two
magnitudes in a type that cannot hold one of them. Spelling it `rT / denom == 0`
would avoid the counterpart and cover every type, but it would check the bound with
the very operator that produced the quotient, and an assertion correlated with what
it checks is the thing the unsigned branches above were relieved of.

The calls these functions make to each other are qualified. Unqualified, ADL adds
the argument's own namespace, where a non-template beats a constrained template
outright — Boost.Int128 has exactly that, a `div` returning its own `i128div_t`
and `u128div_t`, which a structured binding takes apart as happily as xstd's
`div_t`. So `euclidean_div` and `floored_div` had been calling Boost's `div` over
Boost's types, for both signednesses, agreeing on the numbers and skipping every
one of `xstd::div`'s assertions. Returning the result rather than destructuring
it is what turned the substitution into a type error.

`abs` has the signed minimum as a precondition and is total over an unsigned
type, whose `min()` is `0`. `unsigned_abs` returns the unsigned counterpart and
can represent that magnitude at every width. The three divisions require a
nonzero denominator; `MIN / -1` is outside their contract, and is a precondition
only a signed type can reach.

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
behavior. "Integral" rather than "arithmetic", because `is_arithmetic_like` is
the integral-like half *or* floating point and this answers false for every
floating-point type; "operators", because it ranges over a type's operators
rather than its category. It is public because it *is* those six exception
specifications, and a caller asking whether one throws over their own type is
asking exactly this.

Its operations are every requirement `integer_class_type` states over `const`
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
would be, and its exception specification is computed. So `div_t`'s equality and
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

Those aliases cannot demonstrate the widening on their own: the library supplies
their `make_signed_like` and `make_unsigned_like` specializations, so they reach
the arithmetic surface along a path no third party can take. The exact-width test
lists therefore also carry Boost.Int128 and `absl::int128`, which no header names
and nothing specializes for — they are admitted on the strength of their own
operations, and the two trait associations in `test/include/xstd/test/` are the
whole of what a user has to write. Both dependencies are optional; see
[CONTRIBUTING.md](../CONTRIBUTING.md).

The two are not interchangeable, which is why both are here. Boost annotates its
operations and Abseil annotates none, so they land on opposite sides of
`nothrow_integral_operators`. They differ once more: `absl::int128`'s `operator/`
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
- `is_integral_like`, `is_arithmetic_like`, `is_signed_like` and
  `is_unsigned_like` are open counterparts of the standard traits.
- `empty_type` and `conditional_data_member_t` support optional
  `[[no_unique_address]]` storage.
- `to_underlying` forwards a plain enum and preserves one wrapped in
  `std::integral_constant`.
- `nothrow_integral_operators` answers whether the conditional `noexcept` holds.

A concept spelling is provided when the standard library has an analogous
concept; otherwise the trait is the interface. `nothrow_integral_operators` is
the one concept with no trait beside it, having no standard trait to mirror, and
is spelled the way a caller writes it: inside a `noexcept`.

### `to_underlying`

The [original 2016 sketch](ideas.md#1-convenient-underlying-types-for-scoped-enums)
motivated `to_underlying` with scoped enums used as named tuple and array
indices. Rein Halbersma developed the idea and initial usage evidence with
Walter E. Brown; JeanHeyd Meneide then authored
[P1682R1](https://wg21.link/p1682r1) and carried `std::to_underlying` through
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

`xstd::to_chars` widens `std::to_chars` to every integral-like type. Where the
standard library covers the type it *is* that call, so callers get the tuned
implementation; where it does not, the digits are produced here to the same
specification, bases 2 through 36 and `value_too_large` included. The default
base is written as the literal `10`, as [charconv.to.chars] writes it, so the two
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

The two overloads are kept apart by subsumption rather than by hand: the
delegating one requires a `std::to_chars` call to be well-formed *on top of*
`integral_like`, and a conjunction subsumes its left operand, so it wins partial
ordering wherever both are viable. Spelling the other as the negation would work
too, but a negated atomic constraint does not subsume, so exclusivity and
exhaustiveness would become an invariant to maintain across two edits instead of
a property of the constraints. The constraint is that call itself, there being no
standard concept for "the standard library converts this type"; it stays an answer
rather than an error because `I` is the overload's own parameter, which keeps the
expression dependent until the constraint is checked. It is spelled as a named
concept a few lines above rather than inline, so that each overload's
requires-clause is one line and one conjunction — a requires-expression nested
inside a requires-clause reads as neither one thing nor two, and with
`ColumnLimit: 0` its continuation indent is the formatter's to choose.

Their order in the header is reading order: the standard's own call, then the
deletion the standard also makes, then the fallback for what neither covers. It
once could not be. gcov names only the first group of functions sharing a start
line in a file, and gcovr keyed its cross-translation-unit merge on those names,
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
`bool` is integral-like, and `to_chars` is deleted for it as in the standard.

### Formatting

`<xstd/format.hpp>` formats a `div_t` as `(quot, rem)`, through two partial
specializations of `std::formatter`. `div_t` is tuple-like, so where the standard
library can format a tuple of the element type the specialization to use inherits
`std::formatter<std::tuple<I const&, I const&>>` and hands it `std::tie` — the
standard's own rendering, with no intermediate string.

That delegation can be missing in two ways that do not coincide: the element type
may not be formattable, which is the Microsoft STL's 128-bit classes, or tuple
formatting itself may be missing, p2286 having reached libstdc++ only in GCC 15.
Testing `formattable` on the *tuple* covers both with one predicate. Where it
does not hold, the other specialization renders the members through
`xstd::to_chars` into an inherited string formatter, which asks nothing beyond
`integral_like` — so `div_t` formats for every type it accepts, of either
signedness, on every implementation.

The choice is left to partial ordering, on the same footing as `to_chars`'s
overloads: the tuple one requires the base it inherits, spelled where it applies,
and the other requires nothing. Both produce `(quot, rem)`, so which runs is not
observable in the output — only in the spec grammar, `parse()` being inherited.

xstd specializes `std::formatter` only for `div_t`, which is program-defined. The
128-bit types are not xstd's to specialize for: they are built-ins or
standard-library types, which [namespace.std]/2 does not cover. There is no
stream inserter either — a built-in has no associated namespace for ADL, and a
standard-library type's is one no program may add to. Boost.Test, the only
consumer that needed it, asks through `print_log_value`, which the tests
specialize directly.

## Integer division across languages

The names are not consistent across programming languages. The table compares
integer operations with xstd's three conventions. An entry names a matching
quotient/remainder pair unless it says **quotient only** or **remainder only**.
It is a guide to recognizable spellings, not a promise about every language
version or numeric type. Rows with similar sets of conventions are kept
together. The language survey in [P3724R4](https://wg21.link/p3724r4) supplies
additional points of comparison.

| Language | Truncated integer division | Euclidean integer division | Floored integer division |
| :------- | :------------------------- | :------------------------- | :----------------------- |
| C (C99 and later) | `/`, `%` | — | — |
| C++ (C++11 and later) | `/`, `%` | — | — |
| Objective-C | `/`, `%` | — | — |
| C# | `/`, `%` | — | — |
| D | `/`, `%` | — | — |
| F# | `/`, `%` | — | — |
| Go | `/`, `%` | — | — |
| JavaScript `BigInt` | `/`, `%` | — | — |
| PHP | `intdiv`, `%` | — | — |
| Scala | `/`, `%` | — | — |
| Swift | `/`, `%` | — | — |
| Erlang | `div`, `rem` | — | — |
| OCaml | `/`, `mod` | — | — |
| GLSL | `/` (**quotient only**) | — | — |
| Rust | `/`, `%` | `div_euclid`, `rem_euclid` | — |
| Dart | `~/`, `remainder` | `%` (**remainder only**) | — |
| Java | `/`, `%` | — | `Math.floorDiv`, `Math.floorMod` |
| Kotlin | `/`, `%` | — | `floorDiv`, `mod` |
| Zig | `@divTrunc`, `@rem` | — | `@divFloor`, `@mod` |
| Haskell | `quot`, `rem`, `quotRem` | — | `div`, `mod`, `divMod` |
| Julia | `div`, `rem` | — | `fld`, `mod` |
| Common Lisp | `truncate`, `rem` | — | `floor`, `mod` |
| Prolog | `//`, `rem` | — | `div`, `mod` |
| Scheme | R7RS `truncate-quotient`, `truncate-remainder` | R6RS `div`, `mod`, `div-and-mod` | R7RS `floor-quotient`, `floor-remainder` |
| Ada | `/`, `rem` | — | `mod` (**remainder only**) |
| Clojure | `quot`, `rem` | — | `mod` (**remainder only**) |
| Fortran | `/`, `MOD` | — | `MODULO` (**remainder only**) |
| Ruby | `remainder` (**remainder only**) | — | `div`, `%`, `modulo`, `divmod` |
| Python | — | — | `//`, `%`, `divmod` |
| Lua | — | — | `//`, `%` |
| R | — | — | `%/%`, `%%` |
| Standard ML | — | — | `div`, `mod` |
| Perl | — | — | `%` (**remainder only**) |
| CSS | `rem()` (**remainder only**) | — | `mod()` (**remainder only**) |

The paired operations all satisfy `numer == denom * quot + rem`, but choose a
different remainder for negative inputs:

- **Truncated (`quot`/`rem`)** rounds the quotient toward zero; a nonzero
  remainder has the numerator's sign.
- **Euclidean** chooses `0 <= rem < |denom|`, so the remainder is always
  nonnegative.
- **Floored (`div`/`mod`)** rounds the quotient toward negative infinity; a
  nonzero remainder has the denominator's sign.

No single pair of integer operands can make all three results different. For a
positive numerator and negative denominator, truncated and Euclidean division
agree. For a negative numerator and positive denominator, Euclidean and
floored division agree. When both are negative, truncated and floored division
agree; when both are positive, all three agree.

The terminology follows
[Division and Modulus for Computer Scientists](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/divmodnote-letter.pdf).
An operation limited to a positive modulus does not establish the complete
Euclidean convention. This is why older annotations naming Maple and Pascal
are omitted, and why traditional Scheme `modulo` is not placed in the
Euclidean column.

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
