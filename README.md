# Extensions to the C++ Standard Library

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![Lines of Code](https://tokei.rs/b1/github/rhalbersma/xstd?category=code)](https://github.com/rhalbersma/xstd)

| Header                   | Additions          | Description | Reference |
| :-----                   | :--------          | :---------- | :-------- |
| `<xstd/array.hpp>`       | `array_from_types` | Create an `array` from a type list | none |
| `<xstd/cstddef.hpp>`     | `operator "" _uz`  | User-defined literals for `size_t` | [p0330r8](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p0330r8.html) |
| `<xstd/cstdlib.hpp>`     | `abs` <br> `div` <br> `euclidean_div` <br> `floored_div` <br> `sign` | `constexpr std::abs(int)` <br> `constexpr std::div(int, int)` <br> Euclidean division <br> Floored division <br> `constexpr boost::math::sign` | [p0533r6](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p0533r6.pdf) <br> [p0533r6](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p0533r6.pdf) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) <br> [Boost.Math](https://www.boost.org/doc/libs/1_67_0/libs/math/doc/html/math_toolkit/sign_functions.html) |
| `<xstd/type_traits.hpp>` | `is_specialization_of` |Is a type a class template specialization? | [p2098r0](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2098r0.pdf) |
| `<xstd/utility.hpp>`     |  `to_underlying`   | Convert an enum to its underlying type | [p1682r1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1682r1.html) |

## Requirements

These header-only libraries are continuously being tested with the following conforming [C++20](http://www.open-std.org/jtc1/sc22/wg21/prot/14882fdis/n4860.pdf) compilers:

| Platform | Compiler | Versions | Build |
| :------- | :------- | :------- | :---- |
| Linux    | Clang <br> GCC | 10, 11-SVN<br> 10, 11-SVN | [![codecov](https://codecov.io/gh/rhalbersma/xstd/branch/master/graph/badge.svg)](https://codecov.io/gh/rhalbersma/xstd) <br> [![Build Status](https://travis-ci.org/rhalbersma/xstd.svg)](https://travis-ci.org/rhalbersma/xstd) |
| Windows  | Visual Studio  | 2019                      | [![Build status](https://ci.appveyor.com/api/projects/status/nu193iqabu749mpx?svg=true)](https://ci.appveyor.com/project/rhalbersma/xstd) |

## License

Copyright Rein Halbersma 2014-2020.  
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).  
(See accompanying file LICENSE_1_0.txt or copy at [http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt))
