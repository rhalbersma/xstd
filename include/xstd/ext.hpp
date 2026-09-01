//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_EXT_HPP
#define XSTD_EXT_HPP

// IWYU pragma: always_keep

// The adapters below hard-include the libraries they adapt, so this one probes for them:
// it brings whichever the build has, and is empty rather than an error where it has neither.
#if __has_include(<absl/numeric/int128.h>)
#include <xstd/ext/absl.hpp> // IWYU pragma: export; make_signed, make_unsigned
#endif

#if __has_include(<boost/int128.hpp>)
#include <xstd/ext/boost.hpp> // IWYU pragma: export; make_signed, make_unsigned
#endif

#endif // XSTD_EXT_HPP
