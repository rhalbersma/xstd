[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/) 
[![Standard](https://img.shields.io/badge/c%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) 
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0) 
[![](https://tokei.rs/b1/github/rhalbersma/xstd)](https://github.com/rhalbersma/xstd)

Extensions to the C++ Standard Library
--------------------------------------

| Header          | Additions         | Description | Reference |
| :-------------- | :---------------- | ----------- | :---------|
| `<cstddef>`     | `operator "" _zu` | User-defined literals for `size_t` | [P0330R1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0330r1.pdf) |
| `<cstdlib>`     | `abs` <br> `div` <br> `euclidean_div` <br> `floored_div` <br> `sign` | `constexpr std::abs(int)` <br> `constexpr std::div(int, int)` <br> Euclidean instead of truncated division <br> Floored instead of truncated division <br> `constexpr boost::math::sign` | [P0533R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0533r2.pdf) <br> [P0533R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0533r2.pdf) <br> [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division) <br> [Floored division](http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf) <br> [Boost.Math](https://www.boost.org/doc/libs/1_67_0/libs/math/doc/html/math_toolkit/sign_functions.html) |
| `<type_traits>` | `to_underlying_type` | Converts an enum to its underlying type | [Effective Modern C++](http://shop.oreilly.com/product/0636920033707.do), Item 10 | 

Rebooting the `std::bitset` franchise
-------------------------------------

An `int_set<N>` is a modern reimagining of `std::bitset<N>`, keeping what time has proven to be effective, and throwing out what is not. `int_set`s are array-backed sets of integers that are compact and fast: they do less (i.e. they don't do bounds-checking, and they don't throw exceptions) and offer more (e.g. iterators to seamlessly interact with the rest of the Standard Library). This enables you to do your bit-twiddling with familiar syntax, typically leading to cleaner, more expressive code.

Requirements
------------

These header-only libraries are contiuously being tested with the following conforming [C++17](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf) compilers:

| Platform | Compiler       | Versions | Build |
| :------- | :------------- | :------- | :---- |
| Linux    | Clang <br> GCC | 6.0, 7-SVN<br> 7.3, 8.1 | [![codecov](https://codecov.io/gh/rhalbersma/xstd/branch/master/graph/badge.svg)](https://codecov.io/gh/rhalbersma/xstd) <br> [![Build Status](https://travis-ci.org/rhalbersma/xstd.svg)](https://travis-ci.org/rhalbersma/xstd) |
| Windows  | Visual Studio  |                    15.7 | [![Build status](https://ci.appveyor.com/api/projects/status/nu193iqabu749mpx?svg=true)](https://ci.appveyor.com/project/rhalbersma/xstd) |

License
-------

Copyright Rein Halbersma 2014 - 2018.   
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).   
(See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
