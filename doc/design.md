# Design notes

Rationale behind API and CI/toolchain choices that a user of the library
doesn't need in order to call it correctly, but that a contributor or a
curious reader might want. [README.md](../README.md) documents what each
function does and its contract; this file documents *why* it looks the way
it does.

## Design philosophy

Three themes run across the otherwise unrelated headers:

- **`constexpr` all the things.** Every function xstd adds - the division
  family, `abs`/`uabs`/`sign`, `array_from_types`, `to_underlying` -
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
  give each convention its own named function, at every signed integral
  width, so the choice is visible at the call site instead of being buried
  in which arithmetic operator happened to be used.

## API design

### One `std::signed_integral` template per operation, not four overloads

`abs`, `uabs`, `sign`, `div`, `euclidean_div`, `floored_div` and `div_t` are
each a single entity constrained to `std::signed_integral`. They started out
as four fixed-width non-template overloads apiece (`abs`/`labs`/`llabs`/
`imaxabs` and so on), mirroring how `<cstdlib>` itself declares `abs`/`labs`/
`llabs` and `<cinttypes>` declares `imaxabs`. That mirroring bought
`<cstdlib>` familiarity at a price that grew with every operation added:
twenty-odd near-identical functions, each a copy of the same three lines, and
a naming scheme with no name at all for the two narrowest signed widths.

The templates cost none of that and cover more. The exact-width types come
for free - `int8_t`/`int16_t`/`int32_t`/`int64_t` are aliases of `signed
char`/`short`/`int`/`long`/`long long`, so a template over
`std::signed_integral` instantiates for each of them, whichever built-in type
each one happens to name on a given platform. Three follow-on effects were
weighed and accepted:

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

### Why 128-bit integers are not covered

`__int128` is the obvious next width, and the one the template design would
otherwise extend to for free. It is excluded for reasons that are all
external to xstd:

- `std::integral<__int128>` is `false` on libstdc++ outside GNU dialect mode,
  and the project compiles in the strictly conforming dialect
  (`CMAKE_CXX_EXTENSIONS OFF`) on purpose. libc++ answers differently, so
  even the concept check isn't portable.
- `std::make_unsigned_t<__int128>` is a hard error rather than a substitution
  failure on libstdc++ in that same mode, so `uabs`'s return type couldn't be
  spelled with it.
- GCC's `-pedantic-errors`, which the test targets enable, rejects naming
  `__int128` at all.
- MSVC has no 128-bit integer type to cover in the first place.

Supporting it would mean an xstd-local `signed_integral` concept, an
xstd-local `make_unsigned`, and a test translation unit exempted from the
project's own conformance flags - three carve-outs from the strict-conformance
policy in exchange for one width. Should any of those constraints lift, the
change is a one-line swap of the constraint, which is exactly the property
the four-overload design did not have: there it would have been a fifth copy
of every function.

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

### The division contracts' back-multiplication check

The strongest self-check the division functions can make is
`numer == denom * q + r`, and it is only meaningful evaluated in a type wide
enough that the multiplication itself cannot overflow. As four separate
overloads, only `div` could make it - `long long` is guaranteed wide enough to
hold the product of two `int`s, but there was no portable type wider than
`long long`/`intmax_t` to give `ldiv`/`lldiv`/`imaxdiv` the same guarantee, so
those three carried a weaker contract with no way to say so once.

The template states the condition instead of the conclusion:
`sizeof(T) < sizeof(intmax_t)` decides, per instantiation, whether there is a
wider type to check in. Widths that have one now get the check whether or not
they had a name before (`int8_t`, `int16_t`, `int32_t`), and the widest ones
fall back to the sign and magnitude assertions exactly as they used to.

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
