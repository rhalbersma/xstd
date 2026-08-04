//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPE_TRAITS_CONDITIONAL_DATA_MEMBER_HPP
#define XSTD_TYPE_TRAITS_CONDITIONAL_DATA_MEMBER_HPP

#include <xstd/config/no_unique_address.hpp> // XSTD_NO_UNIQUE_ADDRESS
#include <xstd/type_traits/empty_type.hpp>   // empty_type
#include <type_traits>                       // conditional_t

namespace xstd {

// Tag keeps multiple absent [[XSTD_NO_UNIQUE_ADDRESS]] data members distinct.
template<bool Condition, class Type, class Tag>
using conditional_data_member_t = std::conditional_t<Condition, Type, empty_type<Tag>>;

} // namespace xstd

#endif // XSTD_TYPE_TRAITS_CONDITIONAL_DATA_MEMBER_HPP
