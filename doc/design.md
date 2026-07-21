# Design notes

Rationale behind API and CI/toolchain choices that a user of the library
doesn't need in order to call it correctly, but that a contributor or a
curious reader might want. [README.md](../README.md) documents what each
function does and its contract; this file documents *why* it looks the way
it does.

## Design philosophy

Three themes run across the otherwise unrelated headers:

- **`constexpr` all the things.** Every function xstd adds - the division
  family, `abs`/`sign` and siblings, `array_from_types`, `to_underlying` -
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
  give each convention its own named function, at all 4 integer widths,
  so the choice is visible at the call site instead of being buried in
  which arithmetic operator happened to be used.

## API design

### `xstd::abs`/`xstd::labs`/`xstd::llabs`/`xstd::imaxabs`, `xstd::sign`/`xstd::lsign`/`xstd::llsign`/`xstd::imaxsign`

Both families are plain, non-template overloads at 4 fixed widths (`int`,
`long`, `long long`, `intmax_t`), mirroring how `<cstdlib>` itself declares
`abs`/`labs`/`llabs` and `<cinttypes>` declares `imaxabs`: signed integral
arguments only, no unsigned support, no generic template. `xstd::sign` and
friends aren't part of `<cstdlib>`, but were deliberately given the same
narrow, non-template style rather than being written as a single function
template over any type - the way `boost::math::sign` is. A single template
would accept more inputs, but `<cstdlib>`-style non-template overloads keep
overload resolution and error messages simple, and make it obvious this is
a small, closed set of widths rather than an extensible facility.

`intmax_t` and `long` are the same type on some platforms (e.g. 64-bit
Linux) and distinct types on others (e.g. Windows). Naming the `intmax_t`
overloads as unqualified `long`/`intmax_t` overloads of a single function
name would risk a platform-dependent redeclaration/overload collision;
the `imax`-prefixed names sidestep that regardless of platform, matching
how the public family already uses distinct names (`abs`/`labs`/`llabs`/
`imaxabs`) rather than an overload set.

### `xstd::div_t`/`xstd::ldiv_t`/`xstd::lldiv_t`/`xstd::imaxdiv_t` formatting

Each type's `std::formatter` specialization delegates to
`std::formatter<std::tuple<T const&, T const&>>` through `std::tie`, rather
than formatting `quot`/`rem` by hand, so it automatically inherits
whatever formatting C++23 standard libraries give tuple-like values
(delimiters, nesting, etc.) instead of hardcoding a "(quot, rem)" layout
that could drift from what the standard library does elsewhere.

Each type also has a narrow `std::ostream& operator<<` overload (no
wide-character support) that just forwards to `std::format`. This exists
solely so Boost.Test can print these types in test diagnostics on
assertion failure - it is not a general-purpose printing facility.
Application code should prefer `std::format`/`std::print` directly.

### `xstd::is_specialization_of`

Its `Primary` parameter is constrained to `template<class...> class`, so it
only accepts class templates whose parameters are all types. A template
with a non-type parameter, like `std::array` (`template<class, size_t>`),
doesn't evaluate to `false` - passing it as the second argument is a hard
compile error, since its template template parameter kind doesn't match
`template<class...> class`. A fully general version - one that also
handles non-type template parameters - isn't expressible with today's
template template parameter matching rules; C++26 reflection may enable
one.

## CI and toolchain support

### Compiler support policy

Following the model of [apt.llvm.org](https://apt.llvm.org/), xstd
supports the latest two stable releases of each compiler, plus its current
development branch (trunk/preview). Every leg in the CI matrix, including
every trunk/preview leg, is required: a break on trunk fails the workflow
(and its badge in the README) the same as a break on a stable release
does. xstd tracks trunk deliberately rather than treating it as advisory -
the intent is to evolve alongside new compilers rather than discover
breakage only once a compiler goes stable. A weekly toolchain canary invokes
the complete compiler matrix, so changing trunk/preview toolchains and
runner images are also checked when no pull request is active.

### Why some platforms have no trunk/preview leg

`Apple Clang` has no trunk/preview row because Apple doesn't publish
Apple Clang dev snapshots the way LLVM does. The workflow tests the latest
stable Xcode release from each of the two supported series: Apple Clang 17.0.0
from Xcode 16.4 and Apple Clang 21.0.0 from Xcode 26.6. The `Clang-CL`
preview row uses the same `clang-cl.exe` as VS 2026 Stable, but pairs it
with the preview MSVC STL. That makes it a meaningful standard-library test
even though it does not exercise a newer Clang driver.

### MinGW trunk resolution

The `MinGW` workflow pins GCC versions through [WinLibs](https://winlibs.com)
standalone builds rather than a rolling package feed, resolving the
matching release from the GitHub API at run time rather than hardcoding a
release tag that goes stale the moment a new respin ships. Its trunk leg
tracks whatever snapshot build WinLibs currently publishes between stable
branches; when no snapshot currently exists, the leg no-ops rather than
failing (WinLibs only publishes one between stable release branches, so
this is expected some of the time), but a snapshot that exists and fails
to build is a required failure like any other.

### GCC/Clang trunk provisioning

GCC's trunk leg installs Jonathan Wakely's binary snapshot
(https://jwakely.github.io/pkg-gcc-latest/) rather than building GCC from
source. Because that snapshot ships its own, newer libstdc++ (unlike
apt-installed compilers, which all share the system one), linking it
against vcpkg's prebuilt Boost.Test - built by an
ABI-stable, older GCC - breaks Boost.Test's runtime parameter
registration. Building Boost.Test from source with the same trunk compiler
avoids the mismatch; the resulting build is cached, keyed on the exact
trunk snapshot and Boost version, since a from-source rebuild otherwise
costs several minutes on every run. Clang's trunk leg doesn't need this:
apt.llvm.org's Clang packages, including the dev/SVN build, link against
the system libstdc++ like every other apt-installed compiler on the
runner, so there's no separate bundled runtime and no ABI mismatch with
vcpkg's Boost.Test.

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
Apple Clang legs, which use macOS's libc++ by default). The library is
expected to work with any toolchain that implements the C++23 features it
uses, including `std::format` for tuple-like types.
