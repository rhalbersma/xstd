//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_VOID_TYPE_HPP
#define XSTD_TYPE_TRAITS_VOID_TYPE_HPP

namespace xstd {

// An empty base for a class template with nothing to say about its argument.
// Where empty_type stands in for a data member and is therefore comparable,
// constructible like the member it replaces, and tagged to keep two of them
// distinct in a layout, a base needs none of the three: it declares nothing, so
// it inherits nothing into the class that says nothing.
struct void_type
{};

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_VOID_TYPE_HPP
