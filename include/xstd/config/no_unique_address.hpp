//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_CONFIG_NO_UNIQUE_ADDRESS_HPP
#define XSTD_CONFIG_NO_UNIQUE_ADDRESS_HPP

// MSVC keeps the standard spelling layout-neutral for binary compatibility;
// its vendor spelling supplies the standard no_unique_address semantics.
#ifdef _MSC_VER
#define XSTD_NO_UNIQUE_ADDRESS msvc::no_unique_address
#else
#define XSTD_NO_UNIQUE_ADDRESS no_unique_address
#endif

#endif // XSTD_CONFIG_NO_UNIQUE_ADDRESS_HPP
