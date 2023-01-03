# Extensions to the C++ Standard Library

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![Lines of Code](https://tokei.rs/b1/github/rhalbersma/xstd?category=code)](https://github.com/rhalbersma/xstd)

| Header                   | Additions          | Description | Reference |
| :-----                   | :--------          | :---------- | :-------- |
| `<xstd/array.hpp>`       | `array_from_types` | Create an `array` from a type list | none |
| `<xstd/cstddef.hpp>`     | `operator "" _uz`  | User-defined literals for `size_t` | [p0330r8](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p0330r8.html) (C++23) |
| `<xstd/cstdlib.hpp>`     | `abs` <br> `div` <br> `euclidean_div` <br> `floored_div` <br> `sign` | `constexpr std::abs(int)` <br> `constexpr std::div(int, int)` <br> Euclidean division <br> Floored division <br> `constexpr boost::math::sign` | [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23) <br> [p0533r9](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0533r9.pdf) (C++23) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) <br> [Boost.Math](https://www.boost.org/doc/libs/1_80_0/libs/math/doc/html/math_toolkit/sign_functions.html) |
| `<xstd/type_traits.hpp>` | `is_integral_constant` <br> `is_specialization_of` <br> `tagged_empty` <br> `optional_type` | Is a type an `integral_constant`? <br> Is a type a class template specialization? <br> A tagged empty type <br> An optional type | [p2098r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r1.pdf) <br> none <br> none <br> none |
| `<xstd/utility.hpp>`     | `to_underlying`    | Convert an enum to its underlying type | [p1682r3](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1682r3.html) (C++23) |

## Requirements

These header-only libraries are continuously being tested with the following conforming [C++20](http://www.open-std.org/jtc1/sc22/wg21/prot/14882fdis/n4860.pdf) compilers:

| Platform | Compiler       | Versions           | Build |
| :------- | :-------       | :-------           | :---- |
| Linux    | GCC            | 10, 11, 12, 13-SVN | CI currently being ported to GitHub Actions |
| Linux    | Clang          | 13, 14, 15, 16-SVN | CI currently being ported to GitHub Actions |
| Windows  | Visual Studio  | 2019, 2022         | CI currently being ported to GitHub Actions |

## License

Copyright Rein Halbersma 2014-2023.
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).
(See accompanying file LICENSE_1_0.txt or copy at [http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt))
