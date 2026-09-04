//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_TYPE_TRAITS_NO_UNIQUE_ADDRESS_HPP
#define XSTD_MISC_TYPE_TRAITS_NO_UNIQUE_ADDRESS_HPP

// MSVC keeps the standard spelling layout-neutral; its vendor spelling has the semantics.
#ifdef _MSC_VER
#define XSTD_NO_UNIQUE_ADDRESS msvc::no_unique_address
#else
#define XSTD_NO_UNIQUE_ADDRESS no_unique_address
#endif

#endif // XSTD_MISC_TYPE_TRAITS_NO_UNIQUE_ADDRESS_HPP
