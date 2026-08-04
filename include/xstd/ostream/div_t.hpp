//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_OSTREAM_DIV_T_HPP
#define XSTD_OSTREAM_DIV_T_HPP

#include <xstd/concepts/signed_integral_like.hpp> // signed_integral_like
#include <xstd/cstdlib/div_t.hpp>                 // div_t
#include <xstd/format/div_t.hpp>                  // IWYU pragma: keep; formatter<div_t>
#include <format>                                 // format
#include <ostream>                                // ostream

namespace xstd {

// Narrow stream support for test-framework diagnostics.
template<signed_integral_like S>
auto operator<<(std::ostream& ostr, div_t<S> const& d)
        -> std::ostream&
{
        return ostr << std::format("{}", d);
}

} // namespace xstd

#endif // XSTD_OSTREAM_DIV_T_HPP
