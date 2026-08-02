# Design notes

Rationale behind API and CI/toolchain choices that a user of the library
doesn't need in order to call it correctly, but that a contributor or a
curious reader might want. [README.md](../README.md) documents what each
function does and its contract; this file documents *why* it looks the way
it does.

## Design philosophy

Three themes run across the otherwise unrelated headers:

- **`constexpr` all the things.** Every function xstd adds - the division
  family, `abs`/`uabs`/`sign`, `to_underlying`, `aligned_size` -
  is `constexpr`, including the ones that don't strictly need to be for
  their primary use case. A library whose whole point is "make small,
  general-purpose facilities available early" is of limited use if it
  can't be used in a compile-time context; making that the default rather
  than an afterthought is what lets `xstd::euclidean_div` or
  `xstd::is_specialization_of` show up equally naturally in a
  `static_assert` and in ordinary runtime code.
- **`integral_constant` as a first-class overload target, not just a
  trait-metaprogramming detail.** `xstd::to_underlying` doesn't just
  accept a plain enum the way `std::to_underlying` does - it also accepts
  an enum wrapped in `std::integral_constant` and returns the result
  still wrapped, preserving its compile-time-constant-ness rather than
  collapsing it to a runtime value. `xstd::is_integral_constant` exists so
  generic code can detect that wrapping at all. The underlying idea: a
  `std::integral_constant<Enum, E>` is a value of `Enum` that also happens
  to carry its value in the type system, and library functions that take
  `Enum` should be able to take that too, without the caller needing a
  separate, differently-named entry point.
- **Make integer division's rounding convention an explicit, spelled-out
  choice instead of an implicit one.** Built-in `/` and `%` give C++'s
  truncated-toward-zero convention and nothing else; different problems
  want different conventions (Euclidean division's always-nonnegative
  remainder, floored division's divisor-signed remainder), and picking
  the wrong one silently for negative operands is a classic source of
  off-by-one bugs. `xstd::div`/`xstd::euclidean_div`/`xstd::floored_div`
  give each convention its own named function, at every signed integer-like
  width, so the choice is visible at the call site instead of being buried
  in which arithmetic operator happened to be used.

## API design

### One constrained template per operation, not four overloads

`abs`, `uabs`, `sign`, `div`, `euclidean_div`, `floored_div` and `div_t` are
each a single entity constrained to `xstd::signed_integer_like`. They started
out as four fixed-width non-template overloads apiece (`abs`/`labs`/`llabs`/
`imaxabs` and so on), mirroring how `<cstdlib>` itself declares `abs`/`labs`/
`llabs` and `<cinttypes>` declares `imaxabs`. That mirroring bought
`<cstdlib>` familiarity at a price that grew with every operation added:
twenty-odd near-identical functions, each a copy of the same three lines, and
a naming scheme with no name at all for the two narrowest signed widths.

The templates cost none of that and cover more. The exact-width types come
for free - `int8_t`/`int16_t`/`int32_t`/`int64_t` are aliases of `signed
char`/`short`/`int`/`long`/`long long`, so one template instantiates for each
of them, whichever built-in type each one happens to name on a given
platform. Three follow-on effects were weighed and accepted:

- **The result type is now the argument type, not the promoted type.** A call
  to the old non-template `abs(int)` with an `int16_t` argument promoted to
  `int`, so it returned `int` and, incidentally, was in contract even for
  `INT16_MIN`. The template returns `int16_t` and takes `int16_t`'s
  precondition. That is the more defensible contract - the promoting one was
  an artifact of there being no narrow overload to call - and `abs(+x)`
  recovers the old behavior explicitly.
- **Mixed-width calls no longer compile.** `div(T, T)` deduces one `T` from
  both arguments, so `div(8, 3L)` is a deduction failure where the old
  `ldiv(long, long)` would have silently converted. An explicit `div<long>`
  says which width the division happens at, which is the thing that was
  ambiguous.
- **The `imax`-prefixed names are no longer needed.** They existed because
  `intmax_t` and `long` are the same type on some platforms (e.g. 64-bit
  Linux) and distinct on others (e.g. Windows), so a single overload set
  spanning both risked a platform-dependent redeclaration collision. One
  template has no overload set to collide with.

`xstd::sign` isn't part of `<cstdlib>` at all, and is now shaped like
`boost::math::sign`: one template over the argument type, returning a plain
`int` at every width, because a sign is a three-valued quantity rather than a
number in the argument's range.

### Why the constraint is `integer_like` rather than `std::signed_integral`

`std::signed_integral` is spelled over `std::is_integral`, which is not a
property a type can have but a closed list the compiler owns. Two kinds of
type that satisfy every requirement the division family actually places on
its argument are outside that list, and neither can join it:

- **`__int128`**, whose integral-ness is a property of the *dialect* rather
  than of the type. libstdc++'s `is_integral` entry for it is gated on
  `__GLIBCXX_TYPE_INT_N_0`, which GCC and Clang predefine only outside
  `__STRICT_ANSI__` - and this project compiles in the strictly conforming
  dialect (`CMAKE_CXX_EXTENSIONS OFF`) on purpose. The same macro gates
  libstdc++'s `__make_unsigned`, which is why `std::make_unsigned_t<__int128>`
  is a *hard error* there rather than a substitution failure. libc++ makes
  the opposite call and has no dialect gate at all, so the same expression is
  well-formed on one standard library and ill-formed on the other. GCC's
  reason for withholding it is real but unrelated to the type's behavior:
  `intmax_t` is required to be the widest signed integer type and is 64-bit
  for ABI reasons, a contradiction GCC resolves in the standard's favor
  exactly when asked to be standard. In GNU mode it lets the contradiction
  stand, and `std::signed_integral<__int128>` and `sizeof(intmax_t) == 8`
  both hold at once.
- **Integer-class types**: 128-bit *class* types with constexpr `/` and `%`
  and a specialized `std::numeric_limits`. libstdc++ has
  `std::ranges::__detail::__max_diff_type`; the MSVC STL has
  `std::_Signed128` in `<__msvc_int128.hpp>`. No class type can ever satisfy
  `std::is_integral`, on any compiler or dialect, so no amount of flag
  wrangling reaches these.

The standard hit the same wall and answered it with *integer-class types*
([iterator.concept.winc]) - but that answer is another closed list. It is
spelled with `same_as` against each implementation's own reserved names
(libstdc++ does exactly this in `<bits/iterator_concepts.h>`), so it can be
neither reused from outside nor joined by a user's type. xstd therefore needs
its own, open version, which is what `xstd::integer_like` is: a structural
concept that asks what a type *does* - `std::regular`, `std::totally_ordered`,
a specialized `std::numeric_limits` saying `is_integer`, explicit
construction from `int`, and the six arithmetic operators - rather than what
it is called. It is a strict superset of `std::integral`, so every type that
worked before still works, with the same result types and the same values.

Two things this design has to get right, both of which took a false start:

- **The unsigned pairing has to be an open trait too.** `uabs` needs the
  unsigned counterpart of its argument type, and `signed_integer_like` needs
  to *test* for one. `std::make_unsigned` can do neither: its domain is the
  same closed list, and outside it it is ill-formed rather than empty, so
  naming it in a concept turns the check that was supposed to answer "no"
  into a stopped compile. Hence `xstd::unsigned_counterpart`, whose primary
  template is deliberately empty. See below for why it is not called
  `xstd::make_unsigned`.
- **Literals have to be spelled `static_cast<T>(0)`.** An integer-class type
  is only required to be *explicitly* convertible from an integral type
  ([iterator.concept.winc]), so `denom != 0` need not compile for one even
  though `denom != static_cast<T>(0)` does. Every constant in
  `<xstd/cstdlib.hpp>` is written that second way, which is why
  `integer_like` requires `std::constructible_from<T, int>` rather than a
  comparison against a literal. For the built-in widths the two spellings are
  the same expression after promotion.
- **`std::regular` has to lead the conjunction.** `std::numeric_limits`'
  primary template declares a static member function returning `T`, which for
  an array type is ill-formed rather than merely unspecialized - so
  `integer_like<int[3]>` would be a hard error if the `numeric_limits` terms
  were checked first. Putting `std::regular` first rejects arrays,
  references, `void` and function types before `numeric_limits` is ever
  instantiated over them. Four checks in `test/src/concepts.cpp` pin that
  ordering; each becomes a compile error if it is disturbed.

Three claims that were made for the old "128-bit integers are out" position
do not survive contact:

- **`-pedantic-errors` was never the obstacle.** It is dialect-independent -
  it rejects naming `__int128` in GNU mode too - and it is suppressible with
  `#pragma GCC diagnostic ignored "-Wpedantic"`, which is what
  `<xstd/type_traits.hpp>` does around its one `__int128` specialization.
  Switching the project to `gnu++23` would therefore not have helped, and
  would have cost the strict-conformance policy for nothing.
- **MSVC does have a 128-bit integer type.** `std::_Signed128` is not
  integral, has no `std::make_unsigned` and no `std::formatter` - but it is
  precisely an integer-class type, so `signed_integer_like` reaches it with
  one user-written `unsigned_counterpart` specialization.
- **The exemption would not have been needed.** Supporting these types costs
  no carve-out from the project's conformance flags: the headers compile
  clean under `-Weverything -pedantic-errors -Werror`, and the tests name
  `__int128` under the same local pragma the header uses.

What is *not* extended is formatting. `std::formatter<xstd::div_t<T>>`
delegates to the tuple formatter, and a partial specialization's body is
instantiated only at its point of use, so `div_t` over an integer-class type
is simply not formattable unless the user supplies a `std::formatter` for the
element type. That is the existing lazy design doing the right thing without
a new rule.

### `xstd::uabs`

`abs` cannot be total: `|x|` for the most negative value of a signed type is
one past that type's maximum, so there is no signed result to return. The
standard's answer is undefined behavior; xstd's is a precondition. Either
way, callers who need `|x|` for *every* input are left without one.

`uabs` returns the corresponding unsigned type instead, which can represent
that value, so it has no precondition at all. It exists mainly because the
division family's postconditions need it: `div` accepts `denom == MIN`, so a
check of the form `|rem| < |denom|` written with `abs` would trip its own
precondition on a call that is perfectly in contract. A contract check must
not have a narrower domain than the operation it verifies.

The negation is done by unsigned wraparound rather than by widening to a
bigger signed type. Wraparound is well-defined where `-x` on a signed
minimum is not, and it works uniformly at every width - the widest signed
type has nothing to widen to. For types narrower than `int` the subtraction
is evaluated in `int` after promotion and is negative; converting that back
to the unsigned result type reduces it mod 2^N, which is the same value the
wider types get directly.

The name keeps the relationship to `abs` visible at the call site, which
`magnitude` (what this was called while it was an implementation detail) did
not. Rust names the same operation
[`unsigned_abs`](https://doc.rust-lang.org/std/primitive.i32.html#method.unsigned_abs)
for the same reason; C++ has no equivalent, standard or in Boost.Math.

### Why some return types are deduced and the rest are spelled out

Every function in `<xstd/cstdlib.hpp>` with a return type worth naming spells
it out as a trailing return type - except `uabs`, which deduces its own.
`xstd::to_underlying` deduces for the same reason. This is not a style
inconsistency; it is
[CWG2369](https://cplusplus.github.io/CWG/issues/2369.html), "ordering between
constraints and substitution".

A constrained function template's associated constraints should be checked
*before* the deduced arguments are substituted into the rest of the
declaration. GCC does this. Clang did not until
[Clang 21](https://github.com/llvm/llvm-project/pull/122423), which substitutes
first, so a return type that instantiates a trait gets instantiated even for
arguments the constraint would have rejected:

    template<signed_integer_like T>
    constexpr auto uabs(T x) noexcept -> unsigned_counterpart_t<T>;

    static_assert(not has_uabs<double>);   // wrong reason on Clang < 21

`abs` and `sign` are immune because their return types (`T` and `int`)
instantiate nothing; so are `div`, `euclidean_div` and `floored_div`, because
forming `div_t<double>` fails the class template's own constraint in the
immediate context, which *is* a substitution failure.

The stakes here dropped when the constraint moved to `signed_integer_like`.
`std::make_unsigned_t<double>` was ill-formed rather than a substitution
failure, so the line above used to stop the compile instead of evaluating to
`false`. `xstd::unsigned_counterpart` has an empty primary template - being
detectable rather than fatal is the whole reason it exists - so the same line
now merely fails deduction. That is a diagnostic regression rather than a
correctness one, and it is still worth avoiding: a failed constraint reports
that `double` is not a signed integer-like type, while a substitution failure
in the return type reports that a call was not viable, which is true but
tells the caller nothing about why.

Deducing `uabs`'s return type moves the trait from the signature into the
body, where `signed_integer_like` has already rejected the argument. The rule
for this library: spell the return type out unless it instantiates a trait
over a template parameter, in which case deduce it.

This is worth revisiting rather than keeping forever. The `Clang` and
`Clang libc++` legs already run Clang 22 and newer, but `Clang-CL` uses
whichever LLVM Visual Studio bundles - 19.1.5 for VS 2022 and 20.1.8 for
VS 2026 - both of which predate the fix. When the oldest supported Clang is 21
or later, both return types can be spelled out and this note deleted.

### The division contracts' back-multiplication check

`numer == denom * q + r` is the strongest self-check a division function can
make, but it is only worth making once. For truncated division it is a real
check: it holds the built-in `/` and `%` against each other, and it needs no
widening to evaluate, because `denom * qT` is exactly `numer - rT` and `rT`
carries `numer`'s sign, so the product lies between `0` and `numer` inclusive
and is representable wherever `numer` is.

For the two adjusted conventions it is a tautology. Both compute
`q' = qT - I` and `r' = rT + I * denom`, so

    denom * q' + r' = denom * (qT - I) + rT + I * denom = denom * qT + rT

which is `numer` by the check `div` has already made. Asserting it again in
`euclidean_div` and `floored_div` could only fire where `div`'s own assertion
had already fired - and, because the adjusted product *can* exceed `T`, doing
so would have required carrying an `intmax_t` widening and a `sizeof`-based
gate purely to restate something already known. Those two assertions, and the
machinery they needed, are gone. What remains for each convention is what
actually constrains it: the magnitude bound `|r| < |denom|` and its own sign
rule.

### Why `euclidean_div` adds or subtracts `denom` instead of scaling it

Dropping the identity assertion removes the only thing that would have
tripped over `I * denom`, which is worth spelling out because that expression
was unsound on its own terms. `euclidean_div` selects `I == -1` when the
truncated remainder is negative and `denom` is negative, and `denom == MIN` is
in contract - so `I * denom` forms `-MIN`, which is not representable. That is
undefined behavior for `int` and wider; below `int`, integer promotion happens
to evaluate it in `int` and hide it.

The adjustment is therefore spelled as an addition or a subtraction of `denom`
chosen by its sign, which never forms `-denom`:

    qE = denom > 0 ? qT - 1 : qT + 1
    rE = denom > 0 ? rT + denom : rT - denom

Both lines select a whole expression rather than adding a delta, because the
delta they would have to add is `|denom|` - a value that does not exist in `T`
when `denom == MIN`.

Each arm carries its own `static_cast<T>` rather than one cast wrapped around
the whole conditional. For the built-in widths that is the same narrowing in
a different place, and it is how the code was originally written; for an
integer-class type it is the difference between compiling and not. A
conditional operator's operands have to agree on a type before the result can
be cast at all, and `qT - one` is a `T` only once it has been cast - a class
type has no integer promotions to fall back on the way `int8_t` does. So
widening the constraint was not the pure one-line swap the four-overload
design was supposed to have bought; it was a one-line swap plus this.

`floored_div` needs no such care. Its delta is `denom` itself, which is
representable by definition, so `I` is only ever `0` or `1` and both lines can
add a conditional delta instead:

    qF = qT - (adjust ? 1 : 0)
    rF = rT + (adjust ? denom : 0)

where `adjust` is `sign(rT) == -sign(denom)`. The two conventions therefore
look deliberately different: each spells its adjustment in the form its own
representability constraints allow.

### `xstd::div_t` formatting

`div_t`'s `std::formatter` specialization delegates to
`std::formatter<std::tuple<T const&, T const&>>` through `std::tie`, rather
than formatting `quot`/`rem` by hand, so it automatically inherits
whatever formatting C++23 standard libraries give tuple-like values
(delimiters, nesting, etc.) instead of hardcoding a "(quot, rem)" layout
that could drift from what the standard library does elsewhere.

Being a partial specialization over `div_t`'s element type rather than four
explicit specializations has a side benefit: the body is instantiated at the
point of use, so merely including `<xstd/cstdlib.hpp>` no longer requires a
standard library that implements tuple formatting. Only actually formatting
an `xstd::div_t` does.

Formatting is the one xstd operation that isn't `constexpr`, against the
"`constexpr` all the things" theme above, and the reason is entirely
external: `std::formatter<std::tuple<...>>::format` is not `constexpr` in
C++23, so neither can the specialization delegating to it be. Marking it
`constexpr` regardless wouldn't be a harmless white lie - a templated
function no specialization of which could be evaluated in a constant
expression is ill-formed, no diagnostic required ([dcl.constexpr]/6), so
compilers accept it in silence rather than reject it, which is the worst of
both worlds.

[P3391](https://wg21.link/p3391) (`constexpr std::format`) makes the standard
library's own formatter specializations `constexpr`, tuples included, and
announces it with `__cpp_lib_constexpr_format`. The current revision is
[R2](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p3391r2.html)
(2025-11-07), plenary-approved with C++29 as its ship vehicle; standard
libraries usually implement an approved paper well ahead of the standard it
lands in, which is what this gate is waiting for. Inheriting from a `constexpr`-enabled
base is not by itself enough: a format call is a constant expression only if
*every* formatter specialization it uses is `constexpr`-enabled, and
`std::formatter<xstd::div_t<T>>` is one of them. So the header opts in
explicitly, through an `XSTD_CONSTEXPR_FORMAT` macro that expands to
`constexpr` exactly when the feature-test macro is defined and to nothing
otherwise, `#undef`'d immediately after its single use. `parse` needs no such
gate and isn't overridden: the standard formatters' `parse` has had to be
usable in a constant expression since C++20 - that is what compile-time
format-string checking runs on - so the inherited one is already `constexpr`.
None of P3391's exclusions (floating point, locale-aware
formatting, chrono) is reachable from a `div_t` of signed integers.

The tests carry the matching half: `XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL` is
`XSTD_CONSTEXPR_CHECK_EQUAL` where the feature-test macro is defined and a
plain runtime `BOOST_CHECK_EQUAL` where it is not, so the first standard
library to define it turns the formatting checks into `static_assert`s
without any further edit. Without that, the header's macro could quietly
expand to nothing forever and no test would notice. `operator<<` stays
non-`constexpr` under any future standard: `std::ostream` is not a
compile-time facility, and P3391 explicitly leaves the `basic_ostringstream`
side alone.

`div_t` also has a narrow `std::ostream& operator<<` overload (no
wide-character support) that just forwards to `std::format`. This exists
solely so Boost.Test can print the type in test diagnostics on assertion
failure - it is not a general-purpose printing facility. Application code
should prefer `std::format`/`std::print` directly.

An explicit deduction guide is declared even though aggregate class template
argument deduction would already deduce `div_t{q, r}` without one. It makes
the support intentional rather than incidental, and keeps Clang's
`-Wctad-maybe-unsupported` (part of the `-Weverything` the tests build with)
quiet.

### `xstd::is_specialization_of`

[README.md](../README.md) describes the `template<class...> class`
constraint on `Primary` and the hard error a template with a non-type
parameter produces. The reason it is constrained rather than general: a
fully general version - one that also handles non-type template parameters
- isn't expressible with today's template template parameter matching
rules. C++26 reflection may enable one.

### `<xstd/concepts.hpp>`, and when a trait also gets a concept spelling

`<concepts>` names the built-in numeric categories - `integral`,
`signed_integral`, `unsigned_integral`, `floating_point` - but stops there,
so a template wanting an enum has no type-constraint to reach for and has to
fall back on a requires-clause over `std::is_enum_v`. `xstd::enumeration` is
that missing name, and it is what lets `xstd::to_underlying` put its
constraint where it belongs:

    template<enumeration Enum, Enum N>
    constexpr auto to_underlying(std::integral_constant<Enum, N>) noexcept;

This is deliberately *not* a second way to spell the same check. A
type-constraint and a requires-clause in the template head are both
associated constraints, checked at the same point, so swapping one for the
other leaves the CWG2369 ordering discussed above untouched - the return type
stays deduced for its own separate reason. What changes is only that the
restriction reads next to the parameter it restricts.

`xstd::specialization_of` is a different case: it is the constraint form of a
trait that already exists, and it earns its place because the trait cannot
serve that role. A concept can be partially applied in a type-constraint,

    template<specialization_of<std::complex> T> void f(T);

which is the form callers actually want, and which
`is_specialization_of_v<T, std::complex>` has no way to produce. The rule
this sets for the library: a trait gets a concept spelling when the concept
enables a *use* the trait cannot, not merely so that both exist.

Both are named as the noun the type satisfies rather than as `is_enum` /
`is_specialization_of`, following how every concept in `<concepts>` reads.
Beyond consistency, a concept sharing an unqualified name with the trait it
wraps is ambiguous for anyone with using-directives for both namespaces,
which for `enumeration` would have meant colliding with `std::is_enum`.

That same rule is what keeps `integer_like` / `signed_integer_like` /
`unsigned_integer_like` from being spelled `xstd::integral` /
`xstd::signed_integral` / `xstd::unsigned_integral`, which is what they are
generalizations *of*. Those names would collide with `<concepts>`' own under
using-directives for both namespaces, and the collision would be worse than
`is_enum`'s: the two spellings would mean genuinely different things - one a
strict superset of the other - so code that resolved to the wrong one would
compile and be subtly wrong rather than ambiguous. The `_like` suffix also
echoes the standard's own "integer-class type" vocabulary for the same
category of type.

`xstd::unsigned_counterpart` is named the same way, and for a sharper version
of the same reason. It is a generalization of `std::make_unsigned`, so
`xstd::make_unsigned` is the tempting name - but class templates have no
overload resolution to disambiguate them the way `xstd::to_underlying` and
`std::to_underlying` are disambiguated by their argument types. Two class
templates of the same unqualified name from two namespaces in scope are
simply ambiguous, with no way for the caller to mean one of them short of
qualifying. "Counterpart" is also what the standard calls the relationship
("corresponding unsigned type", [basic.fundamental]), so the name says what
the trait computes rather than what it is derived from.

`unsigned_counterpart` is deliberately narrower than `std::make_unsigned` in
one respect: it says nothing about cv-qualified types, where
`std::make_unsigned` carries the qualifiers through. Nothing is lost, because
a cv-qualified type is not `std::regular` - it is not assignable - and so is
never `integer_like` to begin with. What is gained is a domain that is the
same on every platform: the `__int128` specialization names one type, and a
cv-carrying trait would answer for that type's qualified forms only on the
standard libraries where the generic partial specialization happens to match
them too.

### Why `is_integral_constant` does not constrain the wrapped type

`std::integral_constant<T, v>` puts no constraint on `T`: anything usable as
a non-type template parameter works, enums and pointers included. The
obvious-looking tightening -

    template<std::integral U, U N>
    inline constexpr auto is_integral_constant_v<std::integral_constant<U, N>, U> = true;

- would therefore be a narrowing rather than a clarification, and it would
narrow away precisely the case the header is built around: `xstd::to_underlying`
exists to accept `std::integral_constant<Enum, N>`, and the trait exists so
generic code can detect that wrapping at all. Constraining `U` to
`std::integral` would leave the trait blind to it.

Without the constraint the trait is already exact - the primary template is
`false`, and only `std::integral_constant<U, N>` matches the specialization -
so there is nothing over-broad to tighten. The enum case is pinned by a test
rather than left implicit, because every other check in that test uses `bool`
or `int` and so would keep passing if someone made this change anyway.

## CI and toolchain support

### Compiler support policy

[README.md](../README.md) states the policy and lists the versions each
channel currently holds; this section is about why it is shaped that way.

The development channel - upstream's trunk, whatever each project calls it -
is required rather than advisory because the intent is to evolve alongside
new compilers rather than discover breakage only once one reaches a stable
release. The qualification channel exists between the two for the same
reason: a release still under qualification is where a portability problem
is cheapest to report upstream and still get fixed before it ships. A weekly
toolchain canary invokes the complete compiler matrix, so moving development
toolchains and mutable runner images are checked even when no pull request
is active.

The Clang workflows name their apt.llvm.org suite directly - a versioned
`llvm-toolchain-<codename>-<version>` for stable and qualification, the
unversioned `llvm-toolchain-<codename>` for development - rather than
letting `llvm.sh` derive it. That script resolves a version through a
hardcoded table which lags LLVM's annual rollover, so in the weeks around
a release it can neither be asked for the new development version nor find
the previous one in the unversioned suite it resolves to.

### Why some platforms have no development leg

`Apple Clang` has no development row because Apple doesn't publish
Apple Clang dev snapshots the way LLVM does. The workflow tests the latest
stable Xcode release from each of the two supported series: Apple Clang 17.0.0
from Xcode 16.4 and Apple Clang 21.0.0 from Xcode 26.6. The `Clang-CL`
development entry uses the same `clang-cl.exe` as VS 2026 Stable, but pairs it
with the preview MSVC STL. That makes it a meaningful standard-library test
even though it does not exercise a newer Clang driver.

### MinGW development-leg resolution

The `MinGW` workflow pins GCC versions through [WinLibs](https://winlibs.com)
standalone builds rather than a rolling package feed, resolving the
matching release from the GitHub API at run time rather than hardcoding a
release tag that goes stale the moment a new respin ships. Its development
leg tracks whatever snapshot build WinLibs currently publishes between
stable branches; when no snapshot currently exists, the leg no-ops rather than
failing (WinLibs only publishes one between stable release branches, so
this is expected some of the time), but a snapshot that exists and fails
to build is a required failure like any other.

### GCC/Clang development-leg provisioning

GCC's development leg installs Jonathan Wakely's binary snapshot of GCC
trunk (https://jwakely.github.io/pkg-gcc-latest/) rather than building GCC
from source. Because that snapshot ships its own, newer libstdc++ (unlike
apt-installed compilers, which all share the system one), linking it
against vcpkg's prebuilt Boost.Test - built by an ABI-stable, older GCC -
breaks Boost.Test's runtime parameter registration. Building Boost.Test
from source with that same compiler avoids the mismatch; the resulting
build is cached, keyed on the exact snapshot and Boost version, since a
from-source rebuild otherwise costs several minutes on every run. Clang's
development leg doesn't need this: apt.llvm.org's Clang packages, including
the development build, link against the system libstdc++ like every other
apt-installed compiler on the runner, so there's no separate bundled
runtime and no ABI mismatch with vcpkg's Boost.Test.

### MSVC minimum toolset

On Windows, formatting `xstd::div_t` sets the effective minimum toolset:
the C++23 `formatter` specializations for `std::pair` and `std::tuple`
([P2286R8](https://wg21.link/p2286r8)) first shipped in the MSVC STL of
Visual Studio 2022 17.11 (MSVC toolset 19.41), so any VS 2022 release from
17.11 onwards works. The CMake target requests C++23 through
`target_compile_features(... cxx_std_23)`, letting CMake select the
appropriate standard flag for each supported compiler.

### Standard library coverage

All three mainstream standard libraries are exercised: libstdc++ (GCC,
Clang, and MinGW legs), the MSVC STL (MSVC and Clang-CL legs), and libc++
(the Clang-libc++ workflow, which rebuilds Boost.Test against
libc++ through the vcpkg overlay triplet in `.github/vcpkg`, plus the
Apple Clang legs, which use macOS's libc++ by default).
