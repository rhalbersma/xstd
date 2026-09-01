//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/concepts/integer.hpp>
#include <xstd/cstdlib.hpp>
#include <xstd/format.hpp>
#include <xstd/type_traits/is_specialization_of.hpp>
#include <xstd/utility.hpp>
#include <complex>
#include <format>
#include <type_traits>

enum class color : unsigned { red = 1 };

int main()
{
        static_assert(xstd::integer<int>);
        static_assert(xstd::is_specialization_of_v<std::complex<double>, std::complex>);
        static_assert(decltype(xstd::to_underlying(std::integral_constant<color, color::red>()))::value == 1);
        static_assert(xstd::div_euclid(-8, 3).remainder == 1);

        auto const d = xstd::div_floor(-8, 3);
        return std::format("{}", d) == "(-3, 1)" ? 0 : 1;
}
