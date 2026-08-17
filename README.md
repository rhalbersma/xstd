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

xstd is a header-only C++23 library of small, portable standard-library
extensions. All public APIs are in namespace `xstd`.

## Requirements

- A conforming C++23 compiler
- CMake 3.28 or later when using the supplied CMake project
- No third-party runtime or library dependencies

`<xstd/format.hpp>` is the only header that depends on `<format>`.

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
| `<xstd/concepts.hpp>` | `integer_class_operations` <br> `integer_class` <br> `integer_like` <br> `signed_integer_like` <br> `unsigned_integer_like` <br> `nothrow_integer_operators` <br> `specialization_of` | The operations [iterator.concept.winc] states of an integer-class type <br> Those operations, and the signed/unsigned pair this library asks for <br> Constraint form of [iterator.concept.winc]'s *is-integer-like* <br> Open form of `std::signed_integral` <br> Open form of `std::unsigned_integral` <br> Exception specification of the integer functions <br> Constraint form of `is_specialization_of` | [iterator.concept.winc] (integer-class type) <br> [iterator.concept.winc] (integer-class type), [basic.fundamental]/2 <br> [iterator.concept.winc] (integer-class types) <br> [iterator.concept.winc] (integer-class types) <br> [iterator.concept.winc] (integer-class types) <br> none <br> [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (not adopted) |
| `<xstd/charconv.hpp>` | `to_chars` <br> `to_chars_max_size` | `std::to_chars`, widened to any integer-like type <br> Buffer size that holds any value of `T` at any base | [charconv.to.chars] <br> none |
| `<xstd/cstdint.hpp>` | `int128` <br> `uint128` | Platform 128-bit signed integer <br> Platform 128-bit unsigned integer | none <br> none |
| `<xstd/cstdlib.hpp>` | `div_t` <br> `sign` <br> `abs` <br> `unsigned_abs` <br> `div` <br> `euclidean_div` <br> `floored_div` | Defaulted equality comparison <br> `-1`, `0`, or `1`; `0` or `1` when unsigned <br> `constexpr`, any integer-like type <br> Total `\|x\|`, returning the unsigned counterpart <br> Truncated division, any integer-like type <br> Euclidean division <br> Floored division | none <br> [Boost.Math](https://www.boost.org/doc/libs/1_80_0/libs/math/doc/html/math_toolkit/sign_functions.html) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [Rust `unsigned_abs`](https://doc.rust-lang.org/std/primitive.i32.html#method.unsigned_abs) (no C++ equivalent) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23, not yet implemented) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) |
| `<xstd/format.hpp>` | `formatter<div_t>` | `std::format` support for every element type `div_t` accepts | [p3391](https://wg21.link/p3391) (C++29, not yet implemented) |
| `<xstd/memory.hpp>` | `aligned_size` | Round a size up to a power-of-two alignment | none |
| `<xstd/type_traits.hpp>` | `XSTD_NO_UNIQUE_ADDRESS` <br> `empty_type` <br> `is_signed` <br> `is_unsigned` <br> `is_specialization_of` <br> `make_signed` <br> `make_unsigned` <br> `conditional_data_member_t` | Portable spelling of `no_unique_address` <br> A tagged empty type <br> `std::is_signed`, opened to integer-class types <br> `std::is_unsigned`, opened to integer-class types <br> Is a type a class template specialization? <br> Open, user-specializable `std::make_signed` <br> Open, user-specializable `std::make_unsigned` <br> A conditionally present member | none <br> none <br> none <br> none <br> [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) (not adopted) <br> none <br> none <br> none |
| `<xstd/utility.hpp>` | `to_underlying` | `std::to_underlying`, plus an `std::integral_constant` overload | [p1682r1](https://wg21.link/p1682r1) (`std::to_underlying`) |

## Examples

```cpp
#include <climits>
#include <xstd/cstdlib.hpp>
#include <xstd/memory.hpp>

static_assert(xstd::unsigned_abs(INT_MIN) == static_cast<unsigned>(INT_MAX) + 1u);
static_assert(xstd::aligned_size(64, 100) == 128);

constexpr auto result = xstd::euclidean_div(-8, 3);
static_assert(result.quot == -3);
static_assert(result.rem == 1);
```

The numeric concepts and traits extend their standard counterparts to
integer-class types, including the portable 128-bit aliases:

```cpp
#include <xstd/concepts.hpp>
#include <xstd/cstdint.hpp>

static_assert(xstd::signed_integer_like<xstd::int128>);
static_assert(xstd::unsigned_integer_like<xstd::uint128>);
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

The integer functions take any integer-like type of either signedness and
return the argument type rather than a promoted one; a two-argument call
requires both arguments to have the same type. Every one of them has the
counterpart it needs, because an integer-class type is one of a signed/unsigned
pair here: `make_signed` and `make_unsigned` must both name a type for it, which
is automatic in the direction its own signedness answers and a specialization the
type's author writes in the other. A type without that pair is not integer-like,
so a call is unsatisfied rather than ill-formed. `abs` keeps the signed
minimum-value precondition and is total over an unsigned type, whose `min()`
is `0`; `unsigned_abs` is total over both, returning the unsigned counterpart.
The division functions require a nonzero divisor, and `MIN / -1` is outside
their contract. Nothing takes cv `bool`: [iterator.concept.winc] excludes it
from integer-like, a truth value being no one-bit unsigned integer and not
modular the way one would be. The character types are integer-like and are
accepted, as `std::to_chars` accepts them.

A `div_t` renders as `(quot, rem)` for every element type it accepts, but which
format specs it accepts depends on that type: where the standard library can
format a tuple of it the tuple grammar applies, and otherwise a string one.

See [the design notes](doc/design.md) for rationale and for customizing an
integer-class type, and [CONTRIBUTING.md](CONTRIBUTING.md) to build the library itself.

## Continuously tested toolchains

CI covers the stable, qualification, and development channels of the major
C++23 compilers and standard libraries:

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

Copyright Rein Halbersma 2014-2026.
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).
(See accompanying file LICENSE_1_0.txt or copy at [http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt))
