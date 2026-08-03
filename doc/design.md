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
  collapsing it to a runtime value. The underlying idea: a
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
each a single entity constrained to `xstd::signed_integral_like`. They started
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

### Why the constraint is `integral_like` rather than `std::signed_integral`

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
its own, open version, and builds it the way the standard builds its own
answer - in two steps rather than one:

- `xstd::exposition_only::integer_class_type` in
  `<xstd/concepts/exposition_only.hpp>` is the standard's *integer-class type*
  re-derived structurally: a concept that asks what a type *does* -
  `std::regular`, `std::three_way_comparable<I, std::strong_ordering>`, a specialized `std::numeric_limits`
  saying `is_integer`, explicit construction from `int`, and the six
  arithmetic operators - rather than what it is called.
- `xstd::exposition_only::is_integer_like` is then the standard's
  *is-integer-like*: any type other than cv `bool` that is `std::integral`, or
  an integer-class type. `xstd::is_integral_like_v` and `xstd::integral_like`
  expose its trait and constraint spellings, the same way
  `std::integral` is `std::is_integral_v`'s.

Apart from the standard-mandated exclusion of cv `bool`, it includes the
`std::integral` types, including cv-qualified integral types.
That follows C++23's definition directly: `std::integral<T>` reads the
cv-transparent `std::is_integral_v<T>`, so `std::integral<const int>` holds.
The widened category must not introduce a different cv policy.

Two things this design has to get right, both of which took a false start:

- **The signed/unsigned pairing has to be open too.** `uabs` needs the
  unsigned counterpart of its argument type, and generic code needs the
  inverse transformation as well. `std::make_signed` and `std::make_unsigned`
  cannot provide either direction for an integer-class type: their domain is the
  same closed list, and outside it it is ill-formed rather than empty, so
  naming it in a concept turns the check that was supposed to answer "no"
  into a stopped compile. Hence `xstd::make_signed_like` and
  `xstd::make_unsigned_like`, whose primary templates are deliberately empty.
  Their domain is the standard transformations' own -
  every integer-like type except `bool` - opened to the integer-class types,
  in two specializations that split the work the way the standard splits it:
  a built-in integral type forwards to the corresponding standard trait, and
  an integer-*class* type is its own counterpart on the side it already
  occupies. The reverse direction is a one-line specialization inheriting
  from `std::type_identity<Partner>`, in the Boost.MPL metafunction-forwarding
  style. The numeric category itself does not require this operation-specific
  association. See below for the `_like` names.
- **Literals have to be spelled `static_cast<T>(0)`.** An integer-class type
  is only required to be *explicitly* convertible from an integral type
  ([iterator.concept.winc]), so `denom != 0` need not compile for one even
  though `denom != static_cast<T>(0)` does. Every constant in
  `<xstd/cstdlib.hpp>` is written that second way, which is why
  `integer_class_type` requires `std::constructible_from<I, int>` rather than
  a comparison against a literal. For the built-in widths the two spellings are
  the same expression after promotion.
- **The completeness guard has to lead the conjunction.** `std::numeric_limits`'
  primary template declares a static member function returning `I`, which for
  an array type is ill-formed rather than merely unspecialized - so
  `integer_class_type<int[3]>` would be a hard error if the `numeric_limits`
  terms were checked first. `sizeof(I)` first rejects incomplete and
  non-object types before `numeric_limits` is ever instantiated over them;
  the remaining constraints then follow the standard's order, with
  `numeric_limits` last. Checks in `test/src/exposition_only.cpp` pin that
  behavior, and checks in `test/src/concepts.cpp` pin that the public
  concept inherits it; each becomes a compile error if it is disturbed.

Three claims that were made for the old "128-bit integers are out" position
do not survive contact:

- **`-pedantic-errors` was never the obstacle.** All platform-specific type
  selection is isolated in `<xstd/cstdint.hpp>`; every other header consumes
  only `xstd::int128_t` and `xstd::uint128_t`. Switching the project to
  `gnu++23` would therefore not have helped, and would have cost the
  strict-conformance policy for nothing.
- **MSVC does have a 128-bit integer type.** `std::_Signed128` is not
  integral, has no `std::make_unsigned` and no `std::formatter` - but it is
  precisely an integer-class type, so `signed_integral_like` reaches it with
  one user-written `make_unsigned_like` specialization.
- **The exemption would not have been needed.** Supporting these types costs
  no carve-out from the project's conformance flags: the public aliases hide
  the platform representation from traits, algorithms, and their tests.

What is *not* extended is formatting. `std::formatter<xstd::div_t<T>>`
delegates to the tuple formatter, and a partial specialization's body is
instantiated only at its point of use, so `div_t` over an integer-class type
is simply not formattable unless the user supplies a `std::formatter` for the
element type. That is the existing lazy design doing the right thing without
a new rule.

### Integer operations are `noexcept`

Built-in integer operations do not allocate and do not throw. Integer-class types are required to expose the same guarantee syntactically: every construction, comparison, arithmetic, bitwise, shift, compound-assignment, and increment/decrement expression checked by `integer_class_type` is a `noexcept` compound requirement. Prefix and postfix `++` and `--` additionally have the semantic effects of `+= I(1)` and `-= I(1)`, respectively; a requires-expression can verify their signatures and exception specifications but cannot prove that behavioral law. These requirements deliberately exclude heap-allocated and other potentially throwing arbitrary-precision types.

`abs`, `uabs`, `sign`, `div`, `euclidean_div` and `floored_div` are plain, unconditionally `noexcept`. Separate nothrow companion traits would not make that interface guarantee stronger and are not part of the interface.

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

    template<signed_integral_like T>
    constexpr auto uabs(T x) noexcept -> make_unsigned_like_t<T>;

    static_assert(not has_uabs<double>);   // wrong reason on Clang < 21

`abs` and `sign` are immune because their return types (`T` and `int`)
instantiate nothing; so are `div`, `euclidean_div` and `floored_div`, because
forming `div_t<double>` fails the class template's own constraint in the
immediate context, which *is* a substitution failure.

The stakes here dropped when the constraint moved to `signed_integral_like`.
`std::make_unsigned_t<double>` was ill-formed rather than a substitution
failure, so the line above used to stop the compile instead of evaluating to
`false`. `xstd::make_unsigned_like` has an empty primary template - being
detectable rather than fatal is the whole reason it exists - so the same line
now merely fails deduction. That is a diagnostic regression rather than a
correctness one, and it is still worth avoiding: a failed constraint reports
that `double` is not a signed integer-like type, while a substitution failure
in the return type reports that a call was not viable, which is true but
tells the caller nothing about why.

Deducing `uabs`'s return type moves the trait from the signature into the
body, where `signed_integral_like` has already rejected the argument. The rule
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

`xstd::to_underlying` constrains its enum parameter directly with the standard
trait:

    template<class Enum, Enum N>
        requires std::is_enum_v<Enum>
    constexpr auto to_underlying(std::integral_constant<Enum, N>) noexcept;

The requires-clause in the template head is an associated constraint, so the
CWG2369 ordering discussed above remains untouched: the return type stays
deduced for its own separate reason.

`xstd::specialization_of` is a different case: it is the constraint form of a
trait that already exists, and it earns its place because the trait cannot
serve that role. A concept can be partially applied in a type-constraint,

    template<specialization_of<std::complex> T> void f(T);

which is the form callers actually want, and which
`is_specialization_of_v<T, std::complex>` has no way to produce. The rule
this sets for the library: a trait gets a concept spelling when the concept
enables a *use* the trait cannot, not merely so that both exist.

That rule is also why there is no `xstd::arithmetic_like` to go with
`xstd::is_arithmetic_like_v`, and the reasons run in both directions.

The naming rule disposes of it first: a `_like` name exists only where there
is a standard entity to widen, and `<concepts>` has no `std::arithmetic`. It
names `integral`, `signed_integral`, `unsigned_integral` and `floating_point`
and stops - "is this a number" is not a constraint anyone writes, because a
template that can do arithmetic on a type always knows which kind it needs.
This is the mirror image of the missing `is_signed_integral_like_v`, and the
three cases together are the whole rule:

| standard | trait | concept | xstd |
| :------- | :---- | :------ | :--- |
| arithmetic | `std::is_arithmetic` | — | `is_arithmetic_like` only |
| integral | `std::is_integral` | `std::integral` | both |
| signed integral | — | `std::signed_integral` | `signed_integral_like` only |

The use rule disposes of it a second time, and less obviously. Spelled the way
every other concept here is - `concept arithmetic_like = is_arithmetic_like_v<T>;`
- it would be an atomic constraint with no relationship to `integral_like`'s,
so an overload set containing both would go *ambiguous* rather than preferring
the integral one. A concept that looks more general than another and does not
partial-order with it is worse than no concept at all. Spelling it
`integral_like<T> or floating_point_like<T>` instead does partial-order
correctly - a disjunction is subsumed by each of its disjuncts - but it
duplicates the trait's definition in a second place that can drift from it,
which is the arrangement `is_integral_like_v` was just moved *out of*. Neither
price buys a use `requires is_arithmetic_like_v<T>` cannot already serve.

`specialization_of` is named as the noun the type satisfies rather than as
`is_specialization_of`, following how every concept in `<concepts>` reads.
Beyond consistency, a concept sharing an unqualified name with the trait it
wraps is ambiguous for anyone with using-directives for both namespaces.

That same rule is what keeps `integral_like` / `signed_integral_like` /
`unsigned_integral_like` from being spelled `xstd::integral` /
`xstd::signed_integral` / `xstd::unsigned_integral`, which is what they are
generalizations *of*. A concept is looked up like a class template, not like
a function: two same-named ones visible through using-directives for both
namespaces are an *ambiguity*, diagnosed at every unqualified use, with no
overload resolution to pick between them. That is the same failure mode
`xstd::is_enum` would have had, no better and no worse - it is a hard error,
not a silently wrong resolution.

The extra reason to avoid these particular names is not about ambiguity at
all, because ambiguity needs *both* namespaces in scope. It is the
single-directive case: with only `using namespace xstd;` in scope, an
unqualified `signed_integral<T>` resolves quietly to xstd's, and no
diagnostic is issued anywhere. A reader who knows `<concepts>` then reads a
familiar token that means something strictly broader than they think, and a
template constrained with it accepts class types that `std::signed_integral`
would have rejected. Nothing is miscompiled by lookup - lookup is never
wrong here - but the reader is. `signed_integral_like` cannot be misread that
way, and the `_like` suffix additionally echoes the standard's own
"integer-class type" vocabulary for the same category of type.

That gives the library a general rule, which is worth more than any single
name: **an xstd entity that widens a standard one to class types keeps the
standard one's name and appends `_like`.** So `std::integral` becomes
`xstd::integral_like`, `std::is_arithmetic_v` becomes
`xstd::is_arithmetic_like_v`, and `std::make_signed` / `std::make_unsigned`
become `xstd::make_signed_like` / `xstd::make_unsigned_like`. A reader who knows the standard name can derive
the xstd name without looking it up, and the suffix marks exactly what is
different about it - the entity is open where the standard's is closed.

The rule cuts both ways, and that is what keeps the set of names honest: a
`_like` name exists only where there is a standard entity to widen. There is
no `xstd::is_signed_integral_like_v` or `xstd::is_unsigned_integral_like_v`,
because the standard has no `std::is_signed_integral_v` or
`std::is_unsigned_integral_v` - `signed_integral` and `unsigned_integral`
exist only as concepts, spelled over `is_signed_v` and `is_unsigned_v`. xstd
therefore widens exactly those two, and its own signed and unsigned concepts
are spelled over the results the same way the standard's are.

`make_signed_like` and `make_unsigned_like` are where the rule reads least well, and it is worth
saying so: the suffix attaches to the whole trait name rather than to a
predicate, so it says "the `make_unsigned`-like trait" rather than describing
its result, which is not "unsigned-like" but simply unsigned. A name like
`unsigned_counterpart` describes the result better. The rule wins anyway, on
the grounds that one predictable rule beats a set of individually nicer names:
having to remember which extensions were renamed and which were suffixed is a
worse tax than one slightly awkward reading.

The rule also disposes of the collision on its own terms. `xstd::make_signed`
and `xstd::make_unsigned` would have been tempting names, but class templates have no overload
resolution to disambiguate them the way `xstd::to_underlying` and
`std::to_underlying` are disambiguated by their argument types: two class
templates of the same unqualified name from two namespaces in scope are simply
ambiguous, with no way for the caller to mean one of them short of qualifying.

### Opening `is_integral` is what opens the rest

`std::is_arithmetic_v` is the root of the *closed* list. `is_integral_v` and
`is_floating_point_v` feed it; `is_signed_v` and `is_unsigned_v` are spelled
over it (`is_arithmetic_v<T> && T(-1) < T(0)`, and its negation); and none of
them can be true for a class type. It is not, however, the place the list can
be *opened*, and an earlier revision of this branch tried: it made
`is_arithmetic_like_v` the one new judgement, reading
`std::numeric_limits<T>::is_specialized` for a class type, with
`is_signed_like_v` and `is_unsigned_like_v` following from it.

That answered a question the library cannot back up. "Is this type a number"
has no structural content on its own - the closest thing to it is "does it
have a `numeric_limits` specialization", which is a marker, not a behavior -
whereas "is this type an integer" does, and is exactly what
`[iterator.concept.winc]` spells out. So the one new judgement is
`is_integral_like_v`, and the other three are spelled over it:

- `is_arithmetic_like_v` is the standard's own disjunction with the integral
  half replaced - an integer-like type, or a floating-point type. Only the
  integral half is open, because that is the only half xstd has a definition
  for; a class type whose `numeric_limits` says it is *not* an integer is
  therefore not arithmetic-like, where the earlier spelling called it one and
  then had nothing to offer it. Opening the other half later is a second
  exposition-only concept and a second disjunct, with nothing else moving.
- `is_signed_like_v` and `is_unsigned_like_v` then follow by copying the
  standard's own definitions with the opened arithmetic test substituted in.

The public concepts follow [concepts.arithmetic] just as literally. After
substituting the open traits, `signed_integral_like<T>` is
`integral_like<T> && is_signed_like_v<T>`. C++23 defines
`unsigned_integral<T>` as `integral<T> && !signed_integral<T>`, not with
`is_unsigned_v`; simplifying the duplicated integral atom gives
`integral_like<T> && !is_signed_like_v<T>`. This also preserves the shared
atomic `integral_like<T>` constraint needed for subsumption.

An earlier revision also spelled the signedness check as
`std::numeric_limits<T>::is_signed` directly in the concept. That worked, but
it answered a different question in the same place - and it left
`xstd::is_signed_like_v` non-existent, so a caller who wanted the *value* had
nowhere to go. Reaching for `std::is_signed_v` was not an option either: being
`is_arithmetic_v`-based it is false for every class type, so it would have
left `signed_integral_like` rejecting precisely the integer-class types the
concept exists to admit, while still looking correct at every built-in width.

`std::numeric_limits` is still the marker for "this type is a number", read
where it belongs - inside `integer_class_type`, alongside the operators that
say what kind of number. It is the marker the standard already uses that way:
it is specialized for exactly the arithmetic types, and a class type that means
to behave like a number specializes it too.

What the traits above it have to get right is that they must *answer* where the
traits they widen answer, since a variable template's initializer must be
well-formed in every operand where a concept's conjunction merely
short-circuits:

- **`std::numeric_limits<T>` cannot be named for every `T`.** Its primary
  template declares static member functions returning `T`, which is ill-formed
  rather than merely unspecialized for an array type. Naming it inside a
  concept, behind `std::regular`, is what keeps that from reaching the traits
  that read it at all.
- **The guard must not be narrower than the trait it widens.** A widening whose
  domain is smaller than the original's is not a widening.
  `std::is_integral_v` answers `false` for an incomplete type quite happily,
  while `std::destructible` - the first thing `std::regular` asks - requires a
  complete type, and the MSVC STL diagnoses one outright where libstdc++ and
  libc++ happen to answer `false`. A `requires { sizeof(I); }` term therefore
  sits ahead of `std::regular` in `integer_class_type`, and
  `test/src/type_traits.cpp` pins the case for all four traits.

`is_signed_like_v` and `is_unsigned_like_v` each carry a constrained partial
specialization for the same reason, and their guard is now the standard's own
and nothing more: `requires is_arithmetic_like_v<T>`, with everything else
falling to the primary's `false`. An earlier revision needed two extra terms -
`std::constructible_from<T, int>` and strong three-way ordering - because
`is_arithmetic_like_v` was then a `numeric_limits` reading, and a class type
can specialize `numeric_limits` without being constructible from `int` or
ordered, which would have made forming `T(-1)` a hard error rather than a
`false`. Arithmetic-like now *means* integer-like or floating-point, and
`integer_class_type` asks a class type for exactly those two, so the standard's
guard carries them.

Both are the standard's own comparisons, including the unsigned one:
`T(0) < T(-1)`, not "arithmetic and not signed". libstdc++ implements
`is_unsigned` the second way and the two agree wherever `T(-1)` and `T(0)`
differ - which is every type either trait can be asked about - but they are
different questions for a type where those compare equal, and the standard's
spelling is the one that answers *neither signed nor unsigned* there instead
of defaulting to unsigned.

### Which spelling is the definition, and why the exposition-only header exists

Every trait in `<xstd/type_traits.hpp>` that has a concept spelling is the
definition, and the concept in `<xstd/concepts.hpp>` is the one-line reading
of it: `is_specialization_of_v` and `specialization_of`, `is_integral_like_v`
and `integral_like`. That is the direction the standard uses - `std::integral`
is spelled over `std::is_integral_v`, not the reverse - and it is what keeps
the four opened traits together in one header, where a reader who found
`is_arithmetic_like_v` finds the other three beside it. Generic code that wants
a *value* - `std::conjunction`, tag dispatch, an `if constexpr` over a pack -
then has one for each, which is what earns the second spelling its place under
this file's rule that it has to enable a use the first cannot.

`is_integral_like_v` is the one that cannot be *written* that way, and that is
what `<xstd/concepts/exposition_only.hpp>` is for. Three constraints pin the shape:

- **Only a concept's conjunction short-circuits during satisfaction checking.**
  In a variable template's initializer, `A and B` still requires every operand
  to be a well-formed expression, so writing the requirements there directly
  makes `is_integral_like_v<int[3]>` a hard error on
  `std::numeric_limits<int[3]>` rather than the answer `false` - the exact case
  `std::regular` is placed first to protect against. The other three traits
  reach the same safety with a constrained partial specialization, because
  their requirements fit in a requires-clause; `is_integral_like_v`'s do not,
  since they include a requires-expression. So the requirements live in a
  concept one level down, and the trait is a one-line reading of *that*.
- **The concept holding them is not part of the interface.** It answers no
  question `is_integral_like_v` and `integral_like` do not already answer
  publicly, and pinning its exact requirement list as a supported spelling
  would freeze a set that is deliberately smaller than
  `[iterator.concept.winc]`'s. Hence a namespace that says so - the standard's
  own device for a name that exists to make a definition writable rather than
  usable.
- **A concept defined as an atomic constraint over a variable template
  subsumes nothing *on its own*.** `concept signed_integral_like =
  is_signed_integral_like_v<T>;` would be a single atomic constraint with no
  relationship to `integral_like`'s, so an overload set containing both would
  go ambiguous instead of preferring the more constrained one. What makes the
  partial ordering work is that the narrower two are spelled
  `integral_like<T> and ...`, so they contain `integral_like`'s atomic
  constraint verbatim - exactly how `std::signed_integral` is spelled over
  `std::integral`. `test/src/concepts.cpp` pins this with an overload pair
  that would go ambiguous if either were given a flat trait of its own.

For the same reason the signedness test is `std::numeric_limits<T>::is_signed`
rather than `std::is_signed_v<T>`. The latter is
`is_arithmetic_v<T> && T(-1) < T(0)`, and `is_arithmetic_v` is false for every
class type, so it would leave `signed_integral_like` rejecting precisely the
integer-class types the concept exists to admit - while still looking correct
at every built-in width.

`make_signed_like` and `make_unsigned_like` follow their standard counterparts
for cv-qualified built-in integral types and carry their qualifiers through. That is consistent with
the cv-transparent standard category traits and concepts. Integer-class types
are still checked as written: stripping cv from an arbitrary class would test
a different type with a potentially different operator surface.

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
from source. That snapshot ships its own, newer libstdc++, unlike
apt-installed compilers, which all share the system one. Linking it against
a Boost.Test built by an ABI-stable, older GCC breaks Boost.Test's runtime
parameter registration, so Boost.Test has to be compiled by the same
toolchain that compiles the tests. Clang's development leg inherits the
same problem from the other side: apt.llvm.org's Clang links against
whichever libstdc++ it finds, and this leg deliberately points it at the
GCC trunk snapshot (`--gcc-toolchain=/opt/gcc-latest`) so that a
development Clang is tested against a development libstdc++.

Both legs solve it the same way the Clang-libc++ workflow already did:
through a vcpkg overlay triplet in `.github/vcpkg` that chainloads a
toolchain file naming the compiler, so vcpkg builds the ports themselves
with the toolchain under test rather than with whatever the runner image
defaults to. The stable legs use the same triplet, so there is one
provisioning mechanism per workflow rather than one per leg. vcpkg's ABI
hash covers the compiler executable and the toolchain file's contents,
which gives each toolchain its own universe of cached binaries for free -
a development snapshot can never be served a stable leg's Boost.Test.

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
