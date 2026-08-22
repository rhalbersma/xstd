# Extensions to the C++ Standard Library

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-23-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![GCC](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml)
[![MinGW](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml)
[![Clang](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml)
[![Clang-libc++](https://github.com/rhalbersma/xstd/actions/workflows/clang-libc%2B%2B.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-libc%2B%2B.yml)
[![Apple Clang](https://github.com/rhalbersma/xstd/actions/workflows/apple-clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/apple-clang.yml)
[![Clang-CL](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml)
[![MSVC](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml)
[![Coverage](https://codecov.io/gh/rhalbersma/xstd/branch/main/graph/badge.svg)](https://codecov.io/gh/rhalbersma/xstd)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/rhalbersma/xstd/badge)](https://securityscorecards.dev/viewer/?uri=github.com/rhalbersma/xstd)

xstd is a collection of small, portable, header-only standard-library
extensions. It relies on the [C++23](https://wg21.link/N4950) standard and targets the
draft [C++29](https://wg21.link/std) standard library. All public APIs
are in namespace `xstd`.

## Requirements

- A conforming [C++23](https://wg21.link/N4950) compiler
- CMake 3.28 or later when using the supplied CMake project
- No third-party runtime or library dependencies outside `<xstd/ext/>`

`<xstd/format.hpp>` is the only header that depends on `<format>`. The
`<xstd/ext/>` headers are the only ones that include a third-party library, and
no other header includes them: an adapted library is reached by naming it.

## Add xstd to a project

The usual approach is CMake's `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
    xstd
    GIT_REPOSITORY https://github.com/rhalbersma/xstd.git
    GIT_TAG main # Prefer a release tag for reproducible builds.
)
FetchContent_MakeAvailable(xstd)

target_link_libraries(my_target PRIVATE xstd::xstd)
```

`find_package(xstd CONFIG REQUIRED)` and `add_subdirectory(external/xstd)` provide
the same `xstd::xstd` target.

## Headers

| Header | Additions | Description | Reference |
| :----- | :-------- | :---------- | :-------- |
| `<xstd/concepts.hpp>` | `integer_class` <br> `integer` <br> `signed_integer` <br> `unsigned_integer` <br> `nothrow_const_operators` <br> `specialization_of` | The operations [iterator.concept.winc] states of an integer-class type <br> P3701 arithmetic domain, extended to paired integer-class types <br> Open form of `std::signed_integral` <br> Open form of `std::unsigned_integral` <br> Exception specification of the integer functions <br> Constraint form of `is_specialization_of` | [iterator.concept.winc] (integer-class type) <br> [P3701R0](https://wg21.link/P3701R0), [iterator.concept.winc] <br> [iterator.concept.winc] (integer-class types) <br> [iterator.concept.winc] (integer-class types) <br> none <br> [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (relationship documented) |
| `<xstd/charconv.hpp>` | `to_chars` <br> `to_chars_max_size` | `std::to_chars`, widened to xstd integers it does not cover <br> Buffer size that holds any value of `T` at any base | [charconv.to.chars] <br> none |
| `<xstd/cstdint.hpp>` | `bit_int<N>` <br> `bit_uint<N>` <br> `bit_int_max_width` <br> `int128` <br> `uint128` | Native bit-precise signed integer (when available) <br> Native bit-precise unsigned integer (when available) <br> Maximum native bit-precise width (when available) <br> Platform 128-bit signed integer <br> Platform 128-bit unsigned integer | [P3666R0](https://wg21.link/P3666R0) <br> [P3666R0](https://wg21.link/P3666R0) <br> none <br> none <br> none |
| `<xstd/cstdlib.hpp>` | `div_result` <br> `sign` <br> `abs` <br> `unsigned_abs` <br> `div` <br> `div_euclid` <br> `div_floor` | Defaulted equality comparison <br> `-1`, `0`, or `1`; `0` or `1` when unsigned <br> `constexpr`, any xstd integer <br> Total `\|x\|`, returning the unsigned counterpart <br> Truncated division, any xstd integer <br> Euclidean division <br> Floored division | none <br> [Boost.Math](https://www.boost.org/doc/libs/1_80_0/libs/math/doc/html/math_toolkit/sign_functions.html) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (reviewed implementation wording) <br> [Rust `unsigned_abs`](https://doc.rust-lang.org/std/primitive.i32.html#method.unsigned_abs) (no C++ equivalent) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (reviewed implementation wording) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) |
| `<xstd/ext/absl.hpp>` | `make_unsigned<absl::int128>` <br> `make_signed<absl::uint128>` | Pairs Abseil's two integer-class types, so each models `integer` | none <br> none |
| `<xstd/ext/boost.hpp>` | `make_unsigned<boost::int128::int128>` <br> `make_signed<boost::int128::uint128>` | Pairs Boost.Int128's two integer-class types, so each models `integer` | none <br> none |
| `<xstd/format.hpp>` | `formatter<div_result>` | `std::format` support for every element type `div_result` accepts | [p3391](https://wg21.link/P3391R3) (reviewed constexpr-format wording) |
| `<xstd/limits.hpp>` | `numeric_limits` | Open `std::numeric_limits`, specialized for xstd extension types | [numeric.limits] |
| `<xstd/memory.hpp>` | `aligned_size` | Round a size up to a power-of-two alignment | none |
| `<xstd/type_traits.hpp>` | `XSTD_NO_UNIQUE_ADDRESS` <br> `is_character` <br> `empty_type` <br> `is_signed` <br> `is_unsigned` <br> `is_specialization_of` <br> `make_signed` <br> `make_unsigned` <br> `conditional_data_member_t` | Portable spelling of `no_unique_address` <br> Identifies the five non-integer character types <br> A tagged empty type <br> `std::is_signed`, opened to integer-class types <br> `std::is_unsigned`, opened to integer-class types <br> Is a type a class template specialization? <br> Open, user-specializable `std::make_signed` <br> Open, user-specializable `std::make_unsigned` <br> A conditionally present member | none <br> [P3701R0](https://wg21.link/P3701R0) <br> none <br> none <br> none <br> [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (relationship documented) <br> none <br> none <br> none |
| `<xstd/utility.hpp>` | `to_underlying` | `std::to_underlying`, plus an `std::integral_constant` overload | [p1682r1](https://wg21.link/p1682r1) (`std::to_underlying`) |

The native bit-precise aliases are available when the compiler defines
`__BITINT_MAXWIDTH__`; `XSTD_HAS_BIT_INT` detects their public API. Their current
minimum width is two so every exposed type has a signed or unsigned counterpart.
The 128-bit aliases and their `make_signed` and `make_unsigned` associations are
defined together in `<xstd/cstdint/int128.hpp>`; `<xstd/cstdint.hpp>` is their
umbrella. Similarly, `<xstd/type_traits/no_unique_address.hpp>` defines
`XSTD_NO_UNIQUE_ADDRESS`, while `<xstd/type_traits.hpp>` exports it. The two
`<xstd/ext/>` umbrellas export `<xstd/ext/absl/int128.hpp>` and
`<xstd/ext/boost/int128.hpp>`, and each needs the library it adapts on the
include path; nothing above them exports either. Each adapted header is
exported in turn, so including one is enough: `<xstd/ext/absl/int128.hpp>`
brings `absl::int128` and `absl::uint128` with it.

## Examples

```cpp
#include <climits>
#include <xstd/cstdlib.hpp>
#include <xstd/memory.hpp>

static_assert(xstd::unsigned_abs(INT_MIN) == static_cast<unsigned>(INT_MAX) + 1u);
static_assert(xstd::aligned_size(64, 100) == 128);

constexpr auto result = xstd::div_euclid(-8, 3);
static_assert(result.quotient == -3);
static_assert(result.remainder == 1);
```

The numeric concepts, limits, and traits extend their standard counterparts to
integer-class types, including native bit-precise integers and the portable
128-bit aliases:

```cpp
#include <xstd/concepts.hpp>
#include <xstd/cstdint.hpp>
#include <xstd/limits.hpp>

static_assert(xstd::signed_integer<xstd::int128>);
static_assert(xstd::unsigned_integer<xstd::uint128>);

#ifdef XSTD_HAS_BIT_INT
static_assert(xstd::signed_integer<xstd::bit_int<24>>);
static_assert(xstd::numeric_limits<xstd::bit_uint<24>>::digits == 24);
#endif
```

Use `XSTD_NO_UNIQUE_ADDRESS` inside an attribute-specifier. It expands to
`msvc::no_unique_address` with the MSVC-compatible frontend and to
`no_unique_address` elsewhere:

```cpp
#include <xstd/type_traits.hpp>

struct storage {
    [[XSTD_NO_UNIQUE_ADDRESS]] allocator_type allocator;
    value_type value;
};
```

The `integer` concept and its signedness refinements are cv-transparent. The
integer functions return the argument type rather than a promoted one. Following
[P3701R0](https://wg21.link/P3701R0), `bool` and the five character types
`char`, `wchar_t`, `char8_t`, `char16_t`, and `char32_t` are excluded. `signed
char` and `unsigned char` remain integers, preserving exact-width 8-bit aliases.
xstd extends the paper's built-in boundary with paired `integer_class` types and
requires both signed and unsigned transformations. Character conversion delegates
the standard integral domain directly to `std::to_chars`.

`div_result` is the common quotient/remainder result for truncating `div`,
Euclidean `div_euclid`, and floored `div_floor`. Their vocabulary and relevant
rounding semantics follow [P3724R4](https://wg21.link/P3724R4), but xstd
intentionally implements only this established three-operation family.
A `div_result` renders as `(quotient, remainder)` for every element type it accepts, but which
format specs it accepts depends on that type: where the standard library can
format a tuple of it the tuple grammar applies, and otherwise a string one.

See [the design notes](doc/design.md) for rationale and for customizing an
integer-class type, and [CONTRIBUTING.md](CONTRIBUTING.md) to build the library itself.

## Continuous integration

We continuously test the stable, qualification, and development branches of the
major [C++23](https://wg21.link/N4950) toolchains (compilers and standard
libraries) in both Debug and Release mode:

| Platform | Compiler   | Standard Library | Stable                    | Qualification             | Development                    | CI    |
| :------- | :--------- | :--------------- | :------------------------ | :------------------------ | :----------------------------- | :---- |
| Linux    | GCC        | libstdc++        | 15                        | 16                        | 17-SVN                         | [![GCC](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/gcc.yml) |
| Windows  | MinGW      | libstdc++        | 15                        | 16                        | 17-SVN                         | [![MinGW](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/mingw.yml) |
| Linux    | Clang      | libstdc++        | 22 (libstdc++ 15)         | 23 (libstdc++ 16)         | 24-SVN (libstdc++ 17-SVN)      | [![Clang](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang.yml) |
| Linux    | Clang      | libc++           | 22                        | 23                        | 24-SVN                         | [![Clang-libc++](https://github.com/rhalbersma/xstd/actions/workflows/clang-libc%2B%2B.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-libc%2B%2B.yml) |
| macOS    | Apple Clang | libc++          | 17.0.0 (Xcode 16.4)       | 21.0.0 (Xcode 26.6)       | —                              | [![Apple Clang](https://github.com/rhalbersma/xstd/actions/workflows/apple-clang.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/apple-clang.yml) |
| Windows  | Clang-CL   | MSVC             | 19.1.5 (VS 2022)          | 20.1.8 (VS 2026)          | 20.1.8 (VS 2026-Preview)       | [![Clang-CL](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/clang-cl.yml) |
| Windows  | MSVC       | MSVC             | 2022 (17.11+)             | 2026                      | 2026-Preview                   | [![MSVC](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml/badge.svg)](https://github.com/rhalbersma/xstd/actions/workflows/msvc.yml) |

## License

<pre>
         Copyright Rein Halbersma 2014-2026.
Distributed under the <a href="http://www.boost.org/users/license.html">Boost Software License, Version 1.0</a>.
   (See accompanying file LICENSE_1_0.txt or copy at
         <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>)
</pre>
